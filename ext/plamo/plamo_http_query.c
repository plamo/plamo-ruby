#include "plamo.h"
#include "wrapper.h"

VALUE rb_cPlamoHttpQuery;

static void deallocate(Wrapper *wrapper) {
  plamo_http_query_destroy(wrapper->inner);
  free(wrapper);
}

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, deallocate, malloc(sizeof(Wrapper)));
}

static VALUE initialize(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  wrapper->inner = plamo_http_query_new();
  return self;
}

static VALUE push(VALUE self, VALUE key, VALUE value) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  plamo_http_query_add(wrapper->inner, StringValueCStr(key), StringValueCStr(value));
  return Qnil;
}

static VALUE get(VALUE self, VALUE key) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  PlamoStringArray *plamo_string_array = plamo_http_query_get(wrapper->inner, StringValueCStr(key));
  if (plamo_string_array != NULL) {
    VALUE rb_plamo_string_array = Data_Wrap_Struct(rb_cPlamoStringArray, NULL, free, malloc(sizeof(Wrapper)));
    Wrapper *wrapper2;
    Data_Get_Struct(rb_plamo_string_array, Wrapper, wrapper2);
    wrapper2->inner = plamo_string_array;
    return rb_plamo_string_array;
  } else {
    return Qnil;
  }
}

static VALUE delete_at(VALUE self, VALUE key) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  if (plamo_http_query_remove(wrapper->inner, StringValueCStr(key))) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

void Init_plamo_http_query(void) {
  rb_cPlamoHttpQuery = rb_define_class_under(rb_mPlamo, "HttpQuery", rb_cObject);
  rb_define_alloc_func(rb_cPlamoHttpQuery, allocate);
  rb_define_method(rb_cPlamoHttpQuery, "initialize", initialize, 0);
  rb_define_method(rb_cPlamoHttpQuery, "push", push, 2);
  rb_define_method(rb_cPlamoHttpQuery, "[]", get, 1);
  rb_define_method(rb_cPlamoHttpQuery, "delete", delete_at, 1);
}
