#include "plamo.h"
#include "wrapper.h"

VALUE rb_cPlamoFormDataField;

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, free, malloc(sizeof(Wrapper)));
}

static VALUE get_text(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  PlamoFormDataField *plamo_form_data_field = wrapper->inner;
  if (plamo_form_data_field->text) {
    return rb_str_new2(plamo_string_get_char(plamo_form_data_field->text));
  } else {
    return Qnil;
  }
}

static VALUE get_file(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  PlamoFormDataField *plamo_form_data_field = wrapper->inner;
  if (plamo_form_data_field->file) {
    Wrapper *wrapper;
    VALUE rb_plamo_form_data_file = Data_Wrap_Struct(rb_cPlamoFormDataFile, NULL, free, malloc(sizeof(Wrapper)));
    Wrapper *plamo_form_data_file_wrapper;
    Data_Get_Struct(rb_plamo_form_data_file, Wrapper, plamo_form_data_file_wrapper);
    plamo_form_data_file_wrapper->inner = plamo_form_data_field->file;
    return rb_plamo_form_data_file;
  } else {
    return Qnil;
  }
}

static VALUE is_text(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  PlamoFormDataField *plamo_form_data_field = wrapper->inner;
  if (plamo_form_data_field->text) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

static VALUE is_file(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  PlamoFormDataField *plamo_form_data_field = wrapper->inner;
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
