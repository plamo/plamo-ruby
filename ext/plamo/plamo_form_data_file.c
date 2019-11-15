#include "plamo.h"
#include "wrapper.h"

VALUE rb_cPlamoFormDataFile;

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, free, malloc(sizeof(Wrapper)));
}

static VALUE get_content_type(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  const char *str = plamo_form_data_file_get_content_type(wrapper->inner);
  if (str != NULL) {
    return rb_str_new2(str);
  } else {
    return Qnil;
  }
}

static VALUE get_file_name(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  const char *str = plamo_form_data_file_get_file_name(wrapper->inner);
  if (str != NULL) {
    return rb_str_new2(str);
  } else {
    return Qnil;
  }
}

static VALUE get_body(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  const size_t size = plamo_form_data_file_get_body_size(wrapper->inner);
  const unsigned char *body = plamo_form_data_file_get_body(wrapper->inner);
  VALUE rb_array = rb_ary_new2(size);
  for (int i = 0; i < size; i++) {
    rb_ary_store(rb_array, i, CHR2FIX(*(body + i)));
  }
  return rb_array;
}

static VALUE get_body_size(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  return SIZET2NUM(plamo_form_data_file_get_body_size(wrapper->inner));
}

void Init_plamo_form_data_file(void) {
  rb_cPlamoFormDataFile = rb_define_class_under(rb_mPlamo, "FormDataFile", rb_cObject);
  rb_define_alloc_func(rb_cPlamoFormDataFile, allocate);
  rb_define_method(rb_cPlamoFormDataFile, "content_type", get_content_type, 0);
  rb_define_method(rb_cPlamoFormDataFile, "file_name", get_file_name, 0);
  rb_define_method(rb_cPlamoFormDataFile, "size", get_body_size, 0);
  rb_define_method(rb_cPlamoFormDataFile, "body", get_body, 0);
}
