#include "plamo.h"
#include "wrapper.h"

VALUE rb_cPlamoStringArray;

static void deallocate(Wrapper *wrapper) {
  plamo_string_array_destroy(wrapper->inner);
  free(wrapper);
}

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, deallocate, malloc(sizeof(Wrapper)));
}

static VALUE initialize(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  wrapper->inner = plamo_string_array_new();
  return self;
}

static VALUE push(VALUE self, VALUE rb_string) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  plamo_string_array_add(wrapper->inner, StringValueCStr(rb_string));
  return Qnil;
}

static VALUE first(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  const char *str = plamo_string_array_get_first(wrapper->inner);
  if (str != NULL) {
    return rb_str_new2(str);
  } else {
    return Qnil;
  }
}

static VALUE last(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  const char *str = plamo_string_array_get_last(wrapper->inner);
  if (str != NULL) {
    return rb_str_new2(str);
  } else {
    return Qnil;
  }
}

static VALUE get_at(VALUE self, VALUE index) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  const char *str = plamo_string_array_get_at(wrapper->inner, FIX2ULONG(index));
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
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  plamo_string_array_for_each(wrapper->inner, execute_each);
  return Qnil;
}

static VALUE delete_at(VALUE self, VALUE index) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  if (plamo_string_array_remove_at(wrapper->inner, FIX2ULONG(index))) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

void Init_plamo_string_array(void) {
  rb_cPlamoStringArray = rb_define_class_under(rb_mPlamo, "StringArray", rb_cObject);
  rb_define_alloc_func(rb_cPlamoStringArray, allocate);
  rb_define_method(rb_cPlamoStringArray, "initialize", initialize, 0);
  rb_define_method(rb_cPlamoStringArray, "push", push, 1);
  rb_define_method(rb_cPlamoStringArray, "first", first, 0);
  rb_define_method(rb_cPlamoStringArray, "last", last, 0);
  rb_define_method(rb_cPlamoStringArray, "[]", get_at, 1);
  rb_define_method(rb_cPlamoStringArray, "each", each, 0);
  rb_define_method(rb_cPlamoStringArray, "delete_at", delete_at, 1);
}

