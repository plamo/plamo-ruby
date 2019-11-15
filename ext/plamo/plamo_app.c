#include "plamo.h"

VALUE rb_cPlamoApp;

static void deallocate(void *plamo_app) {
  plamo_app_destroy(plamo_app);
}

const rb_data_type_t rb_plamo_app_type = {
  "App",
  {
    NULL,
    deallocate,
    NULL,
  },
  NULL,
  NULL,
  0,
};

static VALUE allocate(VALUE klass) {
  return TypedData_Wrap_Struct(klass, &rb_plamo_app_type, NULL);
}

static VALUE initialize(VALUE self) {
  DATA_PTR(self) = plamo_app_new();
  return self;
}

static VALUE push_middleware(VALUE self, VALUE rb_plamo_middleware) {
  PlamoApp *plamo_app;
  TypedData_Get_Struct(self, PlamoApp, &rb_plamo_app_type, plamo_app);
  PlamoMiddleware *plamo_middleware;
  TypedData_Get_Struct(rb_plamo_middleware, PlamoMiddleware, &rb_plamo_middleware_type, plamo_middleware);
  plamo_app_add_middleware(plamo_app, plamo_middleware);
  return Qnil;
}

static VALUE execute(VALUE self, VALUE rb_plamo_request) {
  PlamoApp *plamo_app;
  TypedData_Get_Struct(self, PlamoApp, &rb_plamo_app_type, plamo_app);
  PlamoRequest *plamo_request;
  TypedData_Get_Struct(self, PlamoRequest, &rb_plamo_request_type, plamo_request);
  VALUE rb_plamo_response = TypedData_Wrap_Struct(rb_cPlamoResponse, &rb_plamo_response_type, plamo_app_execute(plamo_app, plamo_request));
  return rb_plamo_response;
}

void Init_plamo_app(void) {
  rb_cPlamoApp = rb_define_class_under(rb_mPlamo, "App", rb_cObject);
  rb_define_alloc_func(rb_cPlamoApp, allocate);
  rb_define_method(rb_cPlamoApp, "initialize", initialize, 0);
  rb_define_method(rb_cPlamoApp, "push_middleware", push_middleware, 1);
  rb_define_method(rb_cPlamoApp, "execute", execute, 1);
}
