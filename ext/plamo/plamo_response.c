#include "plamo.h"

VALUE rb_cPlamoResponse;

static void deallocate(void *plamo_response) {
  plamo_response_destroy(plamo_response);
}

const rb_data_type_t rb_plamo_response_type = {
  "Response",
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
  return TypedData_Wrap_Struct(klass, &rb_plamo_response_type, NULL);
}

static VALUE initialize(VALUE self) {
  DATA_PTR(self) = plamo_response_new();
  return self;
}

static VALUE status_code(VALUE self) {
  PlamoResponse *plamo_response;
  TypedData_Get_Struct(self, PlamoResponse, &rb_plamo_response_type, plamo_response);
  return UINT2NUM(plamo_response->status_code);
}

static VALUE set_status_code(VALUE self, VALUE code) {
  PlamoResponse *plamo_response;
  TypedData_Get_Struct(self, PlamoResponse, &rb_plamo_response_type, plamo_response);
  plamo_response->status_code = FIX2UINT(code);
  return code;
}

static VALUE header(VALUE self) {
  PlamoResponse *plamo_response;
  TypedData_Get_Struct(self, PlamoResponse, &rb_plamo_response_type, plamo_response);
  VALUE rb_plamo_http_header = TypedData_Wrap_Struct(rb_cPlamoHttpHeader, &rb_plamo_http_header_type, plamo_response->header);
  return rb_plamo_http_header;
}

static VALUE body(VALUE self) {
  PlamoResponse *plamo_response;
  TypedData_Get_Struct(self, PlamoResponse, &rb_plamo_response_type, plamo_response);
  if (plamo_response->body != NULL) {
    VALUE rb_plamo_byte_array = TypedData_Wrap_Struct(rb_cPlamoByteArray, &rb_plamo_byte_array_type, plamo_response->body);
    return rb_plamo_byte_array;
  } else {
    return Qnil;
  }
}

static VALUE set_body(VALUE self, VALUE rb_body) {
  PlamoResponse *plamo_response;
  TypedData_Get_Struct(self, PlamoResponse, &rb_plamo_response_type, plamo_response);
  if (strcmp("String", rb_class2name(rb_obj_class(rb_body))) == 0) {
    struct RString *rstring = RSTRING(rb_body);
    plamo_response->body = plamo_byte_array_new(RSTRING_PTR(rstring), RSTRING_LEN(rstring));
  } else {
    VALUE *array = RARRAY_PTR(rb_body);
    size_t len = RARRAY_LEN(rb_body);
    unsigned char *buf = malloc(sizeof(unsigned char) * len);
    for (size_t i = 0; i < len; i++) {
      buf[i] = NUM2CHR(array[i]);
    }
    plamo_response->body = plamo_byte_array_new(buf, len);
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
