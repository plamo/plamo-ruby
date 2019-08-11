#include "plamo.h"
#include "wrapper.h"

VALUE rb_cPlamoHttpHeader;

static void deallocate(Wrapper *wrapper) {
  plamo_http_header_destroy(wrapper->inner);
  free(wrapper);
}

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, deallocate, malloc(sizeof(Wrapper)));
}

static VALUE initialize(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  wrapper->inner = plamo_http_header_new();
  return self;
}

static VALUE push(VALUE self, VALUE key, VALUE value) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  plamo_http_header_add(wrapper->inner, StringValueCStr(key), StringValueCStr(value));
  return Qnil;
}

static VALUE get(VALUE self, VALUE key) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  PlamoStringArray *plamo_string_array = plamo_http_header_get(wrapper->inner, StringValueCStr(key));
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

static void execute_each(const char *key, PlamoStringArray *values) {
  Wrapper *wrapper;
  VALUE rb_plamo_string_array = Data_Wrap_Struct(rb_cPlamoStringArray, NULL, free, malloc(sizeof(Wrapper)));
  Data_Get_Struct(rb_plamo_string_array, Wrapper, wrapper);
  wrapper->inner = values;
  rb_yield(rb_ary_new3(2, rb_str_new2(key), rb_plamo_string_array));
}

static VALUE each(VALUE self, VALUE key) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  plamo_http_header_for_each(wrapper->inner, execute_each);
  return Qnil;
}

static VALUE delete_at(VALUE self, VALUE key) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  if (plamo_http_header_remove(wrapper->inner, StringValueCStr(key))) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

void Init_plamo_http_header(void) {
  rb_cPlamoHttpHeader = rb_define_class_under(rb_mPlamo, "HttpHeader", rb_cObject);
  rb_define_alloc_func(rb_cPlamoHttpHeader, allocate);
  rb_define_method(rb_cPlamoHttpHeader, "initialize", initialize, 0);
  rb_define_method(rb_cPlamoHttpHeader, "push", push, 2);
  rb_define_method(rb_cPlamoHttpHeader, "[]", get, 1);
  rb_define_method(rb_cPlamoHttpHeader, "each", each, 0);
  rb_define_method(rb_cPlamoHttpHeader, "delete", delete_at, 1);
}
