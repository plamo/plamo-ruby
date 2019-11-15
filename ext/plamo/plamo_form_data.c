#include "plamo.h"

VALUE rb_cPlamoFormData;

static void deallocate(void *plamo_form_data) {
  plamo_form_data_destroy(plamo_form_data);
}

static const rb_data_type_t rb_plamo_form_data_type = {
  "FormData",
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
  return TypedData_Wrap_Struct(klass, &rb_plamo_form_data_type, NULL);
}

static VALUE initialize(VALUE self, VALUE rb_plamo_request) {
  PlamoRequest *plamo_request;
  TypedData_Get_Struct(rb_plamo_request, PlamoRequest, &rb_plamo_request_type, plamo_request);
  PlamoFormData *plamo_form_data = plamo_form_data_new(plamo_request);
  if (plamo_form_data == NULL) {
    return Qnil;
  }
  DATA_PTR(self) = plamo_form_data;
  return self;
}

static void execute_each(const char *key, const PlamoFormDataField *value) {
  VALUE rb_plamo_form_data_field = TypedData_Wrap_Struct(rb_cPlamoFormDataField, &rb_plamo_form_data_field_type, value);
  rb_yield(rb_ary_new3(2, rb_str_new2(key), rb_plamo_form_data_field));
}

static VALUE each(VALUE self) {
  PlamoFormData *plamo_form_data;
  TypedData_Get_Struct(self, PlamoFormData, &rb_plamo_form_data_type, plamo_form_data);
  plamo_form_data_for_each(plamo_form_data, execute_each);
  return Qnil;
}

static VALUE get(VALUE self, VALUE key) {
  PlamoFormData *plamo_form_data;
  TypedData_Get_Struct(self, PlamoFormData, &rb_plamo_form_data_type, plamo_form_data);
  PlamoFormDataFieldArray *plamo_form_data_field_array = plamo_form_data_get(plamo_form_data, StringValueCStr(key));
  if (plamo_form_data_field_array == NULL) {
    return Qnil;
  }
  VALUE rb_plamo_form_data_field_array = TypedData_Wrap_Struct(rb_cPlamoFormDataFieldArray, &rb_plamo_form_data_field_array_type, plamo_form_data_field_array);
  return rb_plamo_form_data_field_array;
}

void Init_plamo_form_data(void) {
  rb_cPlamoFormData = rb_define_class_under(rb_mPlamo, "FormData", rb_cObject);
  rb_define_alloc_func(rb_cPlamoFormData, allocate);
  rb_define_method(rb_cPlamoFormData, "initialize", initialize, 1);
  rb_define_method(rb_cPlamoFormData, "[]", get, 1);
  rb_define_method(rb_cPlamoFormData, "each", each, 0);
}
