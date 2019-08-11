#include "plamo.h"
#include "wrapper.h"

VALUE rb_cPlamoApp;

static void deallocate(Wrapper *wrapper) {
  plamo_app_destroy(wrapper->inner);
  free(wrapper);
}

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, deallocate, malloc(sizeof(Wrapper)));
}

static VALUE initialize(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  wrapper->inner = plamo_app_new();
  return self;
}

static VALUE push_middleware(VALUE self, VALUE rb_plamo_middleware) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  Wrapper *plamo_middleware_wrapper;
  Data_Get_Struct(rb_plamo_middleware, Wrapper, plamo_middleware_wrapper);
  plamo_app_add_middleware(wrapper->inner, plamo_middleware_wrapper->inner);
  return Qnil;
}

static VALUE execute(VALUE self, VALUE rb_plamo_request) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);

  Wrapper *plamo_request_wrapper;
  Data_Get_Struct(rb_plamo_request, Wrapper, plamo_request_wrapper);

  PlamoResponse *response = plamo_app_execute(wrapper->inner, plamo_request_wrapper->inner);
  VALUE rb_plamo_response = Data_Wrap_Struct(rb_cPlamoResponse, NULL, free, malloc(sizeof(Wrapper)));
  Wrapper *plamo_response_wrapper;
  Data_Get_Struct(rb_plamo_response, Wrapper, plamo_response_wrapper);
  plamo_response_wrapper->inner = response;

  return rb_plamo_response;
}

void Init_plamo_app(void) {
  rb_cPlamoApp = rb_define_class_under(rb_mPlamo, "App", rb_cObject);
  rb_define_alloc_func(rb_cPlamoApp, allocate);
  rb_define_method(rb_cPlamoApp, "initialize", initialize, 0);
  rb_define_method(rb_cPlamoApp, "push_middleware", push_middleware, 1);
  rb_define_method(rb_cPlamoApp, "execute", execute, 1);
}
