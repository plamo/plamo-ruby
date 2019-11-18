#include "plamo.h"

VALUE rb_cPlamoFormUrlencoded;

static void deallocate(void *plamo_form_urlencoded) {
  plamo_form_urlencoded_destroy(plamo_form_urlencoded);
}

static const rb_data_type_t rb_plamo_form_urlencoded_type = {
  "FormUrlencoded",
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
  return TypedData_Wrap_Struct(klass, &rb_plamo_form_urlencoded_type, NULL);
}

static VALUE initialize(VALUE self, VALUE request) {
  PlamoRequest *plamo_request;
  TypedData_Get_Struct(request, PlamoRequest, &rb_plamo_request_type, plamo_request);
  DATA_PTR(self) = plamo_form_urlencoded_new(plamo_request);
  return self;
}

static VALUE get(VALUE self, VALUE key) {
  PlamoFormUrlencoded *plamo_form_urlencoded;
  TypedData_Get_Struct(self, PlamoFormUrlencoded, &rb_plamo_form_urlencoded_type, plamo_form_urlencoded);
  const PlamoStringArray *plamo_string_array = plamo_form_urlencoded_get(plamo_form_urlencoded, StringValueCStr(key));
  if (plamo_string_array != NULL) {
    VALUE rb_plamo_string_array = TypedData_Wrap_Struct(rb_cPlamoStringArray, &rb_plamo_string_array_type, (PlamoStringArray*)plamo_string_array);
    OBJ_FREEZE(rb_plamo_string_array);
    return rb_plamo_string_array;
  } else {
    return Qnil;
  }
}

static void execute_each(const char *key, const char *value) {
  rb_yield(rb_ary_new3(2, rb_str_new2(key), rb_str_new2(value)));
}

static VALUE each(VALUE self) {
  PlamoFormUrlencoded *plamo_form_urlencoded;
  TypedData_Get_Struct(self, PlamoFormUrlencoded, &rb_plamo_form_urlencoded_type, plamo_form_urlencoded);
  plamo_form_urlencoded_for_each(plamo_form_urlencoded, execute_each);
  return Qnil;
}

void Init_plamo_form_urlencoded(void) {
  rb_cPlamoFormUrlencoded = rb_define_class_under(rb_mPlamo, "FormUrlencoded", rb_cObject);
  rb_define_alloc_func(rb_cPlamoFormUrlencoded, allocate);
  rb_define_method(rb_cPlamoFormUrlencoded, "initialize", initialize, 1);
  rb_define_method(rb_cPlamoFormUrlencoded, "[]", get, 1);
  rb_define_method(rb_cPlamoFormUrlencoded, "each", each, 0);
}
