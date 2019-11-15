#include "plamo.h"

VALUE rb_cPlamoHttpQuery;

static void deallocate(void *plamo_http_query) {
  plamo_http_query_destroy(plamo_http_query);
}

const rb_data_type_t rb_plamo_http_query_type = {
  "HttpQuery",
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
  return TypedData_Wrap_Struct(klass, &rb_plamo_http_query_type, NULL);
}

static VALUE initialize(VALUE self) {
  DATA_PTR(self) = plamo_http_query_new();
  return self;
}

static VALUE push(VALUE self, VALUE key, VALUE value) {
  PlamoHttpQuery *plamo_http_query;
  TypedData_Get_Struct(self, PlamoHttpQuery, &rb_plamo_http_query_type, plamo_http_query);
  plamo_http_query_add(plamo_http_query, StringValueCStr(key), StringValueCStr(value));
  return Qnil;
}

static VALUE get(VALUE self, VALUE key) {
  PlamoHttpQuery *plamo_http_query;
  TypedData_Get_Struct(self, PlamoHttpQuery, &rb_plamo_http_query_type, plamo_http_query);
  PlamoStringArray *plamo_string_array = plamo_http_query_get(plamo_http_query, StringValueCStr(key));
  if (plamo_string_array != NULL) {
    VALUE rb_plamo_string_array = TypedData_Wrap_Struct(rb_cPlamoStringArray, &rb_plamo_string_array_type, plamo_string_array);
    return rb_plamo_string_array;
  } else {
    return Qnil;
  }
}

static VALUE delete_at(VALUE self, VALUE key) {
  PlamoHttpQuery *plamo_http_query;
  TypedData_Get_Struct(self, PlamoHttpQuery, &rb_plamo_http_query_type, plamo_http_query);
  if (plamo_http_query_remove(plamo_http_query, StringValueCStr(key))) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

void Init_plamo_http_query(void) {
  rb_cPlamoHttpQuery = rb_define_class_under(rb_mPlamo, "HttpQuery", rb_cObject);
  rb_define_alloc_func(rb_cPlamoHttpQuery, allocate);
  rb_define_method(rb_cPlamoHttpQuery, "initialize", initialize, 0);
  rb_define_method(rb_cPlamoHttpQuery, "push", push, 2);
  rb_define_method(rb_cPlamoHttpQuery, "[]", get, 1);
  rb_define_method(rb_cPlamoHttpQuery, "delete", delete_at, 1);
}
