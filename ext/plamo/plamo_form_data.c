#include "plamo.h"
#include "wrapper.h"

VALUE rb_cPlamoFormData;

static void deallocate(Wrapper *wrapper) {
  plamo_form_data_destroy(wrapper->inner);
  free(wrapper);
}

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, deallocate, malloc(sizeof(Wrapper)));
}

static VALUE initialize(VALUE self, VALUE rb_plamo_request) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  Wrapper *plamo_request_wrapper;
  Data_Get_Struct(rb_plamo_request, Wrapper, plamo_request_wrapper);
  PlamoFormData *plamo_form_data = plamo_form_data_new(plamo_request_wrapper->inner);
  if (plamo_form_data == NULL) {
    return Qnil;
  }
  wrapper->inner = plamo_form_data;
  return self;
}

static void execute_each(const char *key, const PlamoFormDataField *value) {
  Wrapper *wrapper;
  VALUE rb_plamo_form_data_field = Data_Wrap_Struct(rb_cPlamoFormDataField, NULL, free, malloc(sizeof(Wrapper)));
  Data_Get_Struct(rb_plamo_form_data_field, Wrapper, wrapper);
  wrapper->inner = value;
  rb_yield(rb_ary_new3(2, rb_str_new2(key), rb_plamo_form_data_field));
}

static VALUE each(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  plamo_form_data_for_each(wrapper->inner, execute_each);
  return Qnil;
}

static VALUE get(VALUE self, VALUE key) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  PlamoFormDataFieldArray *plamo_form_data_field_array = plamo_form_data_get(wrapper->inner, StringValueCStr(key));
  if (plamo_form_data_field_array == NULL) {
    return Qnil;
  }
  VALUE rb_plamo_form_data_field_array = Data_Wrap_Struct(rb_cPlamoFormDataFieldArray, NULL, free, malloc(sizeof(Wrapper)));
  Wrapper *plamo_form_data_field_array_wrapper;
  Data_Get_Struct(rb_plamo_form_data_field_array, Wrapper, plamo_form_data_field_array_wrapper);
  plamo_form_data_field_array_wrapper->inner = plamo_form_data_field_array;
  return rb_plamo_form_data_field_array;
}

void Init_plamo_form_data(void) {
  rb_cPlamoFormData = rb_define_class_under(rb_mPlamo, "FormData", rb_cObject);
  rb_define_alloc_func(rb_cPlamoFormData, allocate);
  rb_define_method(rb_cPlamoFormData, "initialize", initialize, 1);
  rb_define_method(rb_cPlamoFormData, "[]", get, 1);
  rb_define_method(rb_cPlamoFormData, "each", each, 0);
}
