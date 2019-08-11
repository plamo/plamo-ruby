#include "plamo.h"
#include "wrapper.h"

VALUE rb_cPlamoByteArray;

static void deallocate(Wrapper *wrapper) {
  plamo_byte_array_destroy(wrapper->inner);
  free(wrapper);
}

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, deallocate, malloc(sizeof(Wrapper)));
}

static VALUE initialize(VALUE self, VALUE body) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  wrapper->inner = plamo_byte_array_new((const unsigned char*)RARRAY_PTR(body), RARRAY_LEN(body));
  return self;
}

static VALUE get_body(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  return rb_ary_new4(plamo_byte_array_get_body_size(wrapper->inner), (VALUE*)plamo_byte_array_get_body(wrapper->inner));
}

static VALUE get_size(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  return ULONG2NUM(plamo_byte_array_get_body_size(wrapper->inner));
}

void Init_plamo_byte_array(void) {
  rb_cPlamoByteArray = rb_define_class_under(rb_mPlamo, "ByteArray", rb_cObject);
  rb_define_alloc_func(rb_cPlamoByteArray, allocate);
  rb_define_method(rb_cPlamoByteArray, "initialize", initialize, 1);
  rb_define_method(rb_cPlamoByteArray, "body", get_body, 0);
  rb_define_method(rb_cPlamoByteArray, "size", get_size, 0);
}
