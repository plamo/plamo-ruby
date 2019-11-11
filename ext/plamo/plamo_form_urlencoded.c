#include "plamo.h"
#include "wrapper.h"

VALUE rb_cPlamoFormUrlencoded;

static void deallocate(Wrapper *wrapper) {
  plamo_form_urlencoded_destroy(wrapper->inner);
  free(wrapper);
}

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, deallocate, malloc(sizeof(Wrapper)));
}

static VALUE initialize(VALUE self, VALUE request) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  Wrapper *request_wrapper;
  Data_Get_Struct(request, Wrapper, request_wrapper);
  wrapper->inner = plamo_form_urlencoded_new(request_wrapper->inner);
  return self;
}

static VALUE get(VALUE self, VALUE key) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  PlamoStringArray *plamo_string_array = plamo_form_urlencoded_get(wrapper->inner, StringValueCStr(key));
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

static void execute_each(const char *key, const char *value) {
  rb_yield(rb_ary_new3(2, rb_str_new2(key), rb_str_new2(value)));
}

static VALUE each(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  plamo_form_urlencoded_for_each(wrapper->inner, execute_each);
  return Qnil;
}

void Init_plamo_form_urlencoded(void) {
  rb_cPlamoFormUrlencoded = rb_define_class_under(rb_mPlamo, "FormUrlencoded", rb_cObject);
  rb_define_alloc_func(rb_cPlamoFormUrlencoded, allocate);
  rb_define_method(rb_cPlamoFormUrlencoded, "initialize", initialize, 1);
  rb_define_method(rb_cPlamoFormUrlencoded, "[]", get, 1);
  rb_define_method(rb_cPlamoFormUrlencoded, "each", each, 0);
}
