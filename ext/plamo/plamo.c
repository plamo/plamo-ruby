#include "plamo.h"

VALUE rb_mPlamo;

void Init_plamo(void) {
  rb_mPlamo = rb_define_module("Plamo");
  Init_plamo_string_array();
  Init_plamo_http_header();
}
