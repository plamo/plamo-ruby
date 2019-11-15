#include "plamo.h"
#include "wrapper.h"

VALUE rb_cPlamoFormDataFieldArray;

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, free, malloc(sizeof(Wrapper)));
}

static VALUE length(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  return SIZET2NUM(plamo_form_data_field_array_length(wrapper->inner));
}

static void execute_each(const PlamoFormDataField *value) {
  Wrapper *wrapper;
  VALUE rb_plamo_form_data_field = Data_Wrap_Struct(rb_cPlamoFormDataField, NULL, free, malloc(sizeof(Wrapper)));
  Data_Get_Struct(rb_plamo_form_data_field, Wrapper, wrapper);
  wrapper->inner = value;
  rb_yield(rb_plamo_form_data_field);
}

static VALUE each(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  plamo_form_data_field_array_for_each(wrapper->inner, execute_each);
  return Qnil;
}

static VALUE get_at(VALUE self, VALUE index) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  const PlamoFormDataField *plamo_form_data_field = plamo_form_data_field_array_get_at(wrapper->inner, NUM2SIZET(index));
  if (plamo_form_data_field) {
    VALUE rb_plamo_form_data_field = Data_Wrap_Struct(rb_cPlamoFormDataField, NULL, free, malloc(sizeof(Wrapper)));
    Wrapper *plamo_form_data_field_wrapper;
    Data_Get_Struct(rb_plamo_form_data_field, Wrapper, plamo_form_data_field_wrapper);
    plamo_form_data_field_wrapper->inner = plamo_form_data_field;
    return rb_plamo_form_data_field;
  } else {
    return Qnil;
  }
}

static VALUE get_first(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  const PlamoFormDataField *plamo_form_data_field = plamo_form_data_field_array_get_first(wrapper->inner);
  if (plamo_form_data_field) {
    VALUE rb_plamo_form_data_field = Data_Wrap_Struct(rb_cPlamoFormDataField, NULL, free, malloc(sizeof(Wrapper)));
    Wrapper *plamo_form_data_field_wrapper;
    Data_Get_Struct(rb_plamo_form_data_field, Wrapper, plamo_form_data_field_wrapper);
    plamo_form_data_field_wrapper->inner = plamo_form_data_field;
    return rb_plamo_form_data_field;
  } else {
    return Qnil;
  }
}

static VALUE get_last(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  const PlamoFormDataField *plamo_form_data_field = plamo_form_data_field_array_get_last(wrapper->inner);
  if (plamo_form_data_field) {
    VALUE rb_plamo_form_data_field = Data_Wrap_Struct(rb_cPlamoFormDataField, NULL, free, malloc(sizeof(Wrapper)));
    Wrapper *plamo_form_data_field_wrapper;
    Data_Get_Struct(rb_plamo_form_data_field, Wrapper, plamo_form_data_field_wrapper);
    plamo_form_data_field_wrapper->inner = plamo_form_data_field;
    return rb_plamo_form_data_field;
  } else {
    return Qnil;
  }
}

void Init_plamo_form_data_field_array(void) {
  rb_cPlamoFormDataFieldArray = rb_define_class_under(rb_mPlamo, "FormDataFieldArray", rb_cObject);
  rb_define_alloc_func(rb_cPlamoFormDataFieldArray, allocate);
  rb_define_method(rb_cPlamoFormDataFieldArray, "length", length, 0);
  rb_define_alias(rb_cPlamoFormDataFieldArray, "size", "length");
  rb_define_method(rb_cPlamoFormDataFieldArray, "each", each, 0);
  rb_define_method(rb_cPlamoFormDataFieldArray, "[]", get_at, 1);
  rb_define_method(rb_cPlamoFormDataFieldArray, "first", get_first, 0);
  rb_define_method(rb_cPlamoFormDataFieldArray, "last", get_last, 0);
}
