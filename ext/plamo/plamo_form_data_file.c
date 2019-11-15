#include "plamo.h"

VALUE rb_cPlamoFormDataFile;

const rb_data_type_t rb_plamo_form_data_file_type = {
  "FormDataFile",
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
  return TypedData_Wrap_Struct(klass, &rb_plamo_form_data_file_type, NULL);
}

static VALUE get_content_type(VALUE self) {
  PlamoFormDataFile *plamo_form_data_file;
  TypedData_Get_Struct(self, PlamoFormDataFile, &rb_plamo_form_data_file_type, plamo_form_data_file);
  const char *str = plamo_form_data_file_get_content_type(plamo_form_data_file);
  if (str != NULL) {
    return rb_str_new2(str);
  } else {
    return Qnil;
  }
}

static VALUE get_file_name(VALUE self) {
  PlamoFormDataFile *plamo_form_data_file;
  TypedData_Get_Struct(self, PlamoFormDataFile, &rb_plamo_form_data_file_type, plamo_form_data_file);
  const char *str = plamo_form_data_file_get_file_name(plamo_form_data_file);
  if (str != NULL) {
    return rb_str_new2(str);
  } else {
    return Qnil;
  }
}

static VALUE get_body(VALUE self) {
  PlamoFormDataFile *plamo_form_data_file;
  TypedData_Get_Struct(self, PlamoFormDataFile, &rb_plamo_form_data_file_type, plamo_form_data_file);
  const size_t size = plamo_form_data_file_get_body_size(plamo_form_data_file);
  const unsigned char *body = plamo_form_data_file_get_body(plamo_form_data_file);
  VALUE rb_array = rb_ary_new2(size);
  for (int i = 0; i < size; i++) {
    rb_ary_store(rb_array, i, CHR2FIX(*(body + i)));
  }
  return rb_array;
}

static VALUE get_body_size(VALUE self) {
  PlamoFormDataFile *plamo_form_data_file;
  TypedData_Get_Struct(self, PlamoFormDataFile, &rb_plamo_form_data_file_type, plamo_form_data_file);
  return SIZET2NUM(plamo_form_data_file_get_body_size(plamo_form_data_file));
}

void Init_plamo_form_data_file(void) {
  rb_cPlamoFormDataFile = rb_define_class_under(rb_mPlamo, "FormDataFile", rb_cObject);
  rb_define_alloc_func(rb_cPlamoFormDataFile, allocate);
  rb_define_method(rb_cPlamoFormDataFile, "content_type", get_content_type, 0);
  rb_define_method(rb_cPlamoFormDataFile, "file_name", get_file_name, 0);
  rb_define_method(rb_cPlamoFormDataFile, "size", get_body_size, 0);
  rb_define_method(rb_cPlamoFormDataFile, "body", get_body, 0);
}
