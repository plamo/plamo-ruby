#include "plamo.h"

VALUE rb_cPlamoByteArray;

static void deallocate(void *plamo_byte_array) {
  plamo_byte_array_destroy(plamo_byte_array);
}

const rb_data_type_t rb_plamo_byte_array_type = {
  "ByteArray",
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
  return TypedData_Wrap_Struct(klass, &rb_plamo_byte_array_type, NULL);
}

static VALUE initialize(VALUE self, VALUE body) {
  DATA_PTR(self) = plamo_byte_array_new((const unsigned char*)RARRAY_PTR(body), RARRAY_LEN(body));
  return self;
}

static VALUE get_body(VALUE self) {
  PlamoByteArray *plamo_byte_array;
  TypedData_Get_Struct(self, PlamoByteArray, &rb_plamo_byte_array_type, plamo_byte_array);
  const size_t size = plamo_byte_array_get_body_size(plamo_byte_array);
  const unsigned char *body = plamo_byte_array_get_body(plamo_byte_array);
  VALUE rb_array = rb_ary_new2(size);
  for (int i = 0; i < size; i++) {
    rb_ary_store(rb_array, i, CHR2FIX(*(body + i)));
  }
  return rb_array;
}

static VALUE get_size(VALUE self) {
  PlamoByteArray *plamo_byte_array;
  TypedData_Get_Struct(self, PlamoByteArray, &rb_plamo_byte_array_type, plamo_byte_array);
  return ULONG2NUM(plamo_byte_array_get_body_size(plamo_byte_array));
}

void Init_plamo_byte_array(void) {
  rb_cPlamoByteArray = rb_define_class_under(rb_mPlamo, "ByteArray", rb_cObject);
  rb_define_alloc_func(rb_cPlamoByteArray, allocate);
  rb_define_method(rb_cPlamoByteArray, "initialize", initialize, 1);
  rb_define_method(rb_cPlamoByteArray, "body", get_body, 0);
  rb_define_method(rb_cPlamoByteArray, "size", get_size, 0);
}
