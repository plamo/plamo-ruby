#include "plamo.h"

VALUE rb_cPlamoStringArray;

static void deallocate(void *plamo_string_array) {
  plamo_string_array_destroy(plamo_string_array);
}

const rb_data_type_t rb_plamo_string_array_type = {
  "StringArray",
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
  return TypedData_Wrap_Struct(klass, &rb_plamo_string_array_type, NULL);
}

static VALUE initialize(VALUE self) {
  DATA_PTR(self) = plamo_string_array_new();
  return self;
}

static VALUE length(VALUE self) {
  PlamoStringArray *plamo_string_array;
  TypedData_Get_Struct(self, PlamoStringArray, &rb_plamo_string_array_type, plamo_string_array);
  return SIZET2NUM(plamo_string_array_length(plamo_string_array));
}

static VALUE push(VALUE self, VALUE rb_string) {
  PlamoStringArray *plamo_string_array;
  TypedData_Get_Struct(self, PlamoStringArray, &rb_plamo_string_array_type, plamo_string_array);
  if (OBJ_FROZEN(self)) {
    rb_exc_raise(rb_eFrozenError);
    return Qnil;
  }
  plamo_string_array_add(plamo_string_array, StringValueCStr(rb_string));
  return Qnil;
}

static VALUE first(VALUE self) {
  PlamoStringArray *plamo_string_array;
  TypedData_Get_Struct(self, PlamoStringArray, &rb_plamo_string_array_type, plamo_string_array);
  const char *str = plamo_string_array_get_first(plamo_string_array);
  if (str != NULL) {
    return rb_str_new2(str);
  } else {
    return Qnil;
  }
}

static VALUE last(VALUE self) {
  PlamoStringArray *plamo_string_array;
  TypedData_Get_Struct(self, PlamoStringArray, &rb_plamo_string_array_type, plamo_string_array);
  const char *str = plamo_string_array_get_last(plamo_string_array);
  if (str != NULL) {
    return rb_str_new2(str);
  } else {
    return Qnil;
  }
}

static VALUE get_at(VALUE self, VALUE index) {
  PlamoStringArray *plamo_string_array;
  TypedData_Get_Struct(self, PlamoStringArray, &rb_plamo_string_array_type, plamo_string_array);
  const char *str = plamo_string_array_get_at(plamo_string_array, FIX2ULONG(index));
  if (str != NULL) {
    return rb_str_new2(str);
  } else {
    return Qnil;
  }
}

static void execute_each(const char *str) {
  rb_yield(rb_str_new2(str));
}

static VALUE each(VALUE self) {
  PlamoStringArray *plamo_string_array;
  TypedData_Get_Struct(self, PlamoStringArray, &rb_plamo_string_array_type, plamo_string_array);
  plamo_string_array_for_each(plamo_string_array, execute_each);
  return Qnil;
}

static VALUE delete_at(VALUE self, VALUE index) {
  PlamoStringArray *plamo_string_array;
  TypedData_Get_Struct(self, PlamoStringArray, &rb_plamo_string_array_type, plamo_string_array);
  if (plamo_string_array_remove_at(plamo_string_array, FIX2ULONG(index))) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

void Init_plamo_string_array(void) {
  rb_cPlamoStringArray = rb_define_class_under(rb_mPlamo, "StringArray", rb_cObject);
  rb_define_alloc_func(rb_cPlamoStringArray, allocate);
  rb_define_method(rb_cPlamoStringArray, "initialize", initialize, 0);
  rb_define_method(rb_cPlamoStringArray, "length", length, 0);
  rb_define_alias(rb_cPlamoStringArray, "size", "length");
  rb_define_method(rb_cPlamoStringArray, "push", push, 1);
  rb_define_method(rb_cPlamoStringArray, "first", first, 0);
  rb_define_method(rb_cPlamoStringArray, "last", last, 0);
  rb_define_method(rb_cPlamoStringArray, "[]", get_at, 1);
  rb_define_method(rb_cPlamoStringArray, "each", each, 0);
  rb_define_method(rb_cPlamoStringArray, "delete_at", delete_at, 1);
}
