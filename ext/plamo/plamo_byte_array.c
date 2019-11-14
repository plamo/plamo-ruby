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
  const size_t size = plamo_byte_array_get_body_size(wrapper->inner);
  const unsigned char *body = plamo_byte_array_get_body(wrapper->inner);
  VALUE rb_array = rb_ary_new2(size);
  for (int i = 0; i < size; i++) {
    rb_ary_store(rb_array, i, CHR2FIX(*(body + i)));
  }
  return rb_array;
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
