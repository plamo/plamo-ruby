#include "plamo.h"

VALUE rb_cPlamoFormDataFieldArray;

const rb_data_type_t rb_plamo_form_data_field_array_type = {
  "FormDataFieldArray",
  {
    NULL,
    NULL,
    NULL,
  },
  NULL,
  NULL,
  0,
};

static VALUE allocate(VALUE klass) {
  return TypedData_Wrap_Struct(klass, &rb_plamo_form_data_field_array_type, NULL);
}

static VALUE length(VALUE self) {
  PlamoFormDataFieldArray *plamo_form_data_field_array;
  TypedData_Get_Struct(self, PlamoFormDataFieldArray, &rb_plamo_form_data_field_array_type, plamo_form_data_field_array);
  return SIZET2NUM(plamo_form_data_field_array_length(plamo_form_data_field_array));
}

static void execute_each(const PlamoFormDataField *value) {
  VALUE rb_plamo_form_data_field = TypedData_Wrap_Struct(rb_cPlamoFormDataField, &rb_plamo_form_data_field_type, value);
  rb_yield(rb_plamo_form_data_field);
}

static VALUE each(VALUE self) {
  PlamoFormDataFieldArray *plamo_form_data_field_array;
  TypedData_Get_Struct(self, PlamoFormDataFieldArray, &rb_plamo_form_data_field_array_type, plamo_form_data_field_array);
  plamo_form_data_field_array_for_each(plamo_form_data_field_array, execute_each);
  return Qnil;
}

static VALUE get_at(VALUE self, VALUE index) {
  PlamoFormDataFieldArray *plamo_form_data_field_array;
  TypedData_Get_Struct(self, PlamoFormDataFieldArray, &rb_plamo_form_data_field_array_type, plamo_form_data_field_array);
  const PlamoFormDataField *plamo_form_data_field = plamo_form_data_field_array_get_at(plamo_form_data_field_array, NUM2SIZET(index));
  if (plamo_form_data_field) {
    VALUE rb_plamo_form_data_field = TypedData_Wrap_Struct(rb_cPlamoFormDataField, &rb_plamo_form_data_field_type, plamo_form_data_field);
    return rb_plamo_form_data_field;
  } else {
    return Qnil;
  }
}

static VALUE get_first(VALUE self) {
  PlamoFormDataFieldArray *plamo_form_data_field_array;
  TypedData_Get_Struct(self, PlamoFormDataFieldArray, &rb_plamo_form_data_field_array_type, plamo_form_data_field_array);
  const PlamoFormDataField *plamo_form_data_field = plamo_form_data_field_array_get_first(plamo_form_data_field_array);
  if (plamo_form_data_field) {
    VALUE rb_plamo_form_data_field = TypedData_Wrap_Struct(rb_cPlamoFormDataField, &rb_plamo_form_data_field_type, plamo_form_data_field);
    return rb_plamo_form_data_field;
  } else {
    return Qnil;
  }
}

static VALUE get_last(VALUE self) {
  PlamoFormDataFieldArray *plamo_form_data_field_array;
  TypedData_Get_Struct(self, PlamoFormDataFieldArray, &rb_plamo_form_data_field_array_type, plamo_form_data_field_array);
  const PlamoFormDataField *plamo_form_data_field = plamo_form_data_field_array_get_last(plamo_form_data_field_array);
  if (plamo_form_data_field) {
    VALUE rb_plamo_form_data_field = TypedData_Wrap_Struct(rb_cPlamoFormDataField, &rb_plamo_form_data_field_type, plamo_form_data_field);
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
