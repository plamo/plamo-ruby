#include "plamo.h"
#include "wrapper.h"
#include <stdbool.h>
#include <pthread.h>
#include <ruby/thread.h>

// This hack is reference the below post.
// https://www.burgestrand.se//articles/asynchronous-callbacks-in-ruby-c-extensions/

VALUE rb_cPlamoMiddleware;

typedef struct PlamoRbCallbackConfig {
  VALUE config;
  VALUE callback;
} PlamoRbCallbackConfig;

typedef struct callback_t {
  bool handled;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  struct callback_t *next;
  const PlamoRbCallbackConfig *config;
  const PlamoRequest *plamo_request;
  PlamoResponse *plamo_response;
} callback_t;

typedef struct callback_waiting_t {
  callback_t *callback;
  bool abort;
} callback_waiting_t;

pthread_mutex_t g_callback_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_callback_cond = PTHREAD_COND_INITIALIZER;
callback_t *g_callback_queue = NULL;

static void g_callback_queue_push(callback_t *callback) {
  callback->next = g_callback_queue;
  g_callback_queue = callback;
}

static callback_t* g_callback_queue_pop(void) {
  callback_t *callback = g_callback_queue;
  if (callback) {
    g_callback_queue = callback->next;
  }
  return callback;
}

static void deallocate(Wrapper *wrapper) {
  plamo_middleware_destroy((PlamoMiddleware*)wrapper->inner);
  free(wrapper);
}

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, deallocate, malloc(sizeof(Wrapper)));
}

static void* plamo_callback(void *args) {
  VALUE rb_array = (VALUE)args;
  const PlamoRbCallbackConfig *config = (const PlamoRbCallbackConfig*)rb_ary_shift(rb_array);
  const PlamoRequest *plamo_request = (const PlamoRequest*)rb_ary_shift(rb_array);
  PlamoResponse *plamo_response = (PlamoResponse*)rb_ary_shift(rb_array);
  callback_t callback = {
    .config = config,
    .plamo_request = plamo_request,
    .plamo_response = plamo_response,
    .handled = false,
  };
  pthread_mutex_init(&callback.mutex, NULL);
  pthread_cond_init(&callback.cond, NULL);

  pthread_mutex_lock(&g_callback_mutex);
  g_callback_queue_push(&callback);
  pthread_mutex_unlock(&g_callback_mutex);

  pthread_cond_signal(&g_callback_cond);

  pthread_mutex_lock(&callback.mutex);
  while (callback.handled == false) {
    pthread_cond_wait(&callback.cond, &callback.mutex);
  }
  pthread_mutex_unlock(&callback.mutex);

  pthread_mutex_destroy(&callback.mutex);
  pthread_cond_destroy(&callback.cond);

  return NULL;
}

static void before_plamo_callback(const void *config, const PlamoRequest *plamo_request, PlamoResponse *plamo_response) {
  rb_thread_call_without_gvl(plamo_callback, (void*)rb_ary_new3(3, config, plamo_request, plamo_response), NULL, NULL);
  return;
}

static VALUE handle_callback(void *cb) {
  callback_t *callback = (callback_t*)cb;

  VALUE rb_config = callback->config->config;
  VALUE rb_proc = callback->config->callback;

  VALUE rb_plamo_request = Data_Wrap_Struct(rb_cPlamoRequest, NULL, free, malloc(sizeof(Wrapper)));
  Wrapper *plamo_request_wrapper;
  Data_Get_Struct(rb_plamo_request, Wrapper, plamo_request_wrapper);
  plamo_request_wrapper->inner = (PlamoRequest*)callback->plamo_request;
  VALUE rb_plamo_response = Data_Wrap_Struct(rb_cPlamoResponse, NULL, free, malloc(sizeof(Wrapper)));
  Wrapper *plamo_response_wrapper;
  Data_Get_Struct(rb_plamo_response, Wrapper, plamo_response_wrapper);
  plamo_response_wrapper->inner = callback->plamo_response;

  rb_proc_call(rb_proc, rb_ary_new3(3, rb_config, rb_plamo_request, rb_plamo_response));

  pthread_mutex_lock(&callback->mutex);
  callback->handled = true;
  pthread_cond_signal(&callback->cond);
  pthread_mutex_unlock(&callback->mutex);

  return Qnil;
}

static void* wait_for_callback_signal(void *w) {
  callback_waiting_t *waiting = (callback_waiting_t*)w;

  pthread_mutex_lock(&g_callback_mutex);

  while (waiting->abort == false && (waiting->callback = g_callback_queue_pop()) == NULL) {
    pthread_cond_wait(&g_callback_cond, &g_callback_mutex);
  }

  pthread_mutex_unlock(&g_callback_mutex);

  return NULL;
}

static void stop_waiting_for_callback_signal(void *w) {
  callback_waiting_t *waiting = (callback_waiting_t*)w;

  pthread_mutex_lock(&g_callback_mutex);
  waiting->abort = true;
  pthread_cond_signal(&g_callback_cond);
  pthread_mutex_unlock(&g_callback_mutex);
}

static VALUE event_thread(void *_) {
  callback_waiting_t waiting = {
    .callback = NULL,
    .abort = false
  };

  while (waiting.abort == false) {
    rb_thread_call_without_gvl(wait_for_callback_signal, &waiting, stop_waiting_for_callback_signal, &waiting);

    if (waiting.callback) {
      rb_thread_create(handle_callback, (void*)waiting.callback);
    }
  }

  return Qnil;
}

static VALUE initialize(VALUE self, VALUE rb_config, VALUE rb_callback) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  PlamoRbCallbackConfig *plamo_rb_calback_config = malloc(sizeof(PlamoRbCallbackConfig));
  plamo_rb_calback_config->config = rb_config;
  plamo_rb_calback_config->callback = rb_callback;
  wrapper->inner = plamo_middleware_new(plamo_rb_calback_config, before_plamo_callback);

  return self;
}

void Init_plamo_middleware(void) {
  rb_cPlamoMiddleware = rb_define_class_under(rb_mPlamo, "Middleware", rb_cObject);
  rb_define_alloc_func(rb_cPlamoMiddleware, allocate);
  rb_define_method(rb_cPlamoMiddleware, "initialize", initialize, 2);
  rb_thread_create(event_thread, NULL);
}
