#include "plamo.h"
#include "wrapper.h"

VALUE rb_cPlamoResponse;

static void deallocate(Wrapper *wrapper) {
  plamo_response_destroy(wrapper->inner);
  free(wrapper);
}

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, deallocate, malloc(sizeof(Wrapper)));
}

static VALUE initialize(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  wrapper->inner = plamo_response_new();
  return self;
}

static VALUE status_code(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  return UINT2NUM(((PlamoResponse*)wrapper->inner)->status_code);
}

static VALUE set_status_code(VALUE self, VALUE code) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  ((PlamoResponse*)wrapper->inner)->status_code = FIX2UINT(code);
  return code;
}

static VALUE header(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  VALUE rb_plamo_http_header = Data_Wrap_Struct(rb_cPlamoHttpHeader, NULL, free, malloc(sizeof(Wrapper)));
  Wrapper *plamo_http_header_wrapper;
  Data_Get_Struct(rb_plamo_http_header, Wrapper, plamo_http_header_wrapper);
  plamo_http_header_wrapper->inner = ((PlamoResponse*)wrapper->inner)->header;
  return rb_plamo_http_header;
}

static VALUE body(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  if (((PlamoResponse*)wrapper->inner)->body != NULL) {
    VALUE rb_plamo_byte_array = Data_Wrap_Struct(rb_cPlamoByteArray, NULL, free, malloc(sizeof(Wrapper)));
    Wrapper *plamo_byte_array_wrapper;
    Data_Get_Struct(rb_plamo_byte_array, Wrapper, plamo_byte_array_wrapper);
    plamo_byte_array_wrapper->inner = ((PlamoResponse*)wrapper->inner)->body;
    return rb_plamo_byte_array;
  } else {
    return Qnil;
  }
}

static VALUE set_body(VALUE self, VALUE rb_body) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);

  if (strcmp("String", rb_class2name(rb_obj_class(rb_body))) == 0) {
    struct RString *rstring = RSTRING(rb_body);
    ((PlamoResponse*)wrapper->inner)->body = plamo_byte_array_new(RSTRING_PTR(rstring), RSTRING_LEN(rstring));
  } else {
    VALUE *array = RARRAY_PTR(rb_body);
    size_t len = RARRAY_LEN(rb_body);
    unsigned char *buf = malloc(sizeof(unsigned char) * len);
    for (size_t i = 0; i < len; i++) {
      buf[i] = NUM2CHR(array[i]);
    }
    ((PlamoResponse*)wrapper->inner)->body = plamo_byte_array_new(buf, len);
    free(array);
  }

  return self;
}

void Init_plamo_response(void) {
  rb_cPlamoResponse = rb_define_class_under(rb_mPlamo, "Response", rb_cObject);
  rb_define_alloc_func(rb_cPlamoResponse, allocate);
  rb_define_method(rb_cPlamoResponse, "initialize", initialize, 0);
  rb_define_method(rb_cPlamoResponse, "status_code", status_code, 0);
  rb_define_method(rb_cPlamoResponse, "status_code=", set_status_code, 1);
  rb_define_method(rb_cPlamoResponse, "header", header, 0);
  rb_define_method(rb_cPlamoResponse, "body", body, 0);
  rb_define_method(rb_cPlamoResponse, "body=", set_body, 1);
}
