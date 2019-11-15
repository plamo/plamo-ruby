#include "plamo.h"

VALUE rb_cPlamoFormDataField;

const rb_data_type_t rb_plamo_form_data_field_type = {
  "FormDataField",
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
  return TypedData_Wrap_Struct(klass, &rb_plamo_form_data_field_type, NULL);
}

static VALUE get_text(VALUE self) {
  PlamoFormDataField *plamo_form_data_field;
  TypedData_Get_Struct(self, PlamoFormDataFile, &rb_plamo_form_data_field_type, plamo_form_data_field);
  if (plamo_form_data_field->text) {
    return rb_str_new2(plamo_string_get_char(plamo_form_data_field->text));
  } else {
    return Qnil;
  }
}

static VALUE get_file(VALUE self) {
  PlamoFormDataField *plamo_form_data_field;
  TypedData_Get_Struct(self, PlamoFormDataFile, &rb_plamo_form_data_field_type, plamo_form_data_field);
  if (plamo_form_data_field->file) {
    VALUE rb_plamo_form_data_file = TypedData_Wrap_Struct(rb_cPlamoFormDataFile, &rb_plamo_form_data_file_type, plamo_form_data_field->file);
    return rb_plamo_form_data_file;
  } else {
    return Qnil;
  }
}

static VALUE is_text(VALUE self) {
  PlamoFormDataField *plamo_form_data_field;
  TypedData_Get_Struct(self, PlamoFormDataFile, &rb_plamo_form_data_field_type, plamo_form_data_field);
  if (plamo_form_data_field->text) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

static VALUE is_file(VALUE self) {
  PlamoFormDataField *plamo_form_data_field;
  TypedData_Get_Struct(self, PlamoFormDataFile, &rb_plamo_form_data_field_type, plamo_form_data_field);
  if (plamo_form_data_field->file) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

void Init_plamo_form_data_field(void) {
  rb_cPlamoFormDataField = rb_define_class_under(rb_mPlamo, "FormDataField", rb_cObject);
  rb_define_alloc_func(rb_cPlamoFormDataField, allocate);
  rb_define_method(rb_cPlamoFormDataField, "text", get_text, 0);
  rb_define_method(rb_cPlamoFormDataField, "file", get_file, 0);
  rb_define_method(rb_cPlamoFormDataField, "text?", is_text, 0);
  rb_define_method(rb_cPlamoFormDataField, "file?", is_file, 0);
}
