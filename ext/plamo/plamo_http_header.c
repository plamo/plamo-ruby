#include "plamo.h"

VALUE rb_cPlamoHttpHeader;

static void deallocate(void *plamo_http_header) {
  plamo_http_header_destroy(plamo_http_header);
}

const rb_data_type_t rb_plamo_http_header_type = {
  "HttpHeader",
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
  return TypedData_Wrap_Struct(klass, &rb_plamo_http_header_type, NULL);
}

static VALUE initialize(VALUE self) {
  DATA_PTR(self) = plamo_http_header_new();
  return self;
}

static VALUE push(VALUE self, VALUE key, VALUE value) {
  PlamoHttpHeader *plamo_http_header;
  TypedData_Get_Struct(self, PlamoHttpHeader, &rb_plamo_http_header_type, plamo_http_header);
  plamo_http_header_add(plamo_http_header, StringValueCStr(key), StringValueCStr(value));
  return Qnil;
}

static VALUE get(VALUE self, VALUE key) {
  PlamoHttpHeader *plamo_http_header;
  TypedData_Get_Struct(self, PlamoHttpHeader, &rb_plamo_http_header_type, plamo_http_header);
  PlamoStringArray *plamo_string_array = plamo_http_header_get(plamo_http_header, StringValueCStr(key));
  if (plamo_string_array != NULL) {
    VALUE rb_plamo_string_array = TypedData_Wrap_Struct(rb_cPlamoStringArray, &rb_plamo_string_array_type, plamo_string_array);
    return rb_plamo_string_array;
  } else {
    return Qnil;
  }
}

static void execute_each(const char *key, const char *value) {
  rb_yield(rb_ary_new3(2, rb_str_new2(key), rb_str_new2(value)));
}

static VALUE each(VALUE self) {
  PlamoHttpHeader *plamo_http_header;
  TypedData_Get_Struct(self, PlamoHttpHeader, &rb_plamo_http_header_type, plamo_http_header);
  plamo_http_header_for_each(plamo_http_header, execute_each);
  return Qnil;
}

static VALUE delete_at(VALUE self, VALUE key) {
  PlamoHttpHeader *plamo_http_header;
  TypedData_Get_Struct(self, PlamoHttpHeader, &rb_plamo_http_header_type, plamo_http_header);
  if (plamo_http_header_remove(plamo_http_header, StringValueCStr(key))) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

void Init_plamo_http_header(void) {
  rb_cPlamoHttpHeader = rb_define_class_under(rb_mPlamo, "HttpHeader", rb_cObject);
  rb_define_alloc_func(rb_cPlamoHttpHeader, allocate);
  rb_define_method(rb_cPlamoHttpHeader, "initialize", initialize, 0);
  rb_define_method(rb_cPlamoHttpHeader, "push", push, 2);
  rb_define_method(rb_cPlamoHttpHeader, "[]", get, 1);
  rb_define_method(rb_cPlamoHttpHeader, "each", each, 0);
  rb_define_method(rb_cPlamoHttpHeader, "delete", delete_at, 1);
}
