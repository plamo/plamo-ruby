#include "plamo.h"

VALUE rb_mPlamo;

void Init_plamo(void) {
  rb_mPlamo = rb_define_module("Plamo");
  Init_plamo_string_array();
  Init_plamo_byte_array();
  Init_plamo_http_header();
  Init_plamo_http_query();
  Init_plamo_request();
  Init_plamo_response();
  Init_plamo_form_urlencoded();
  Init_plamo_form_data_file();
  Init_plamo_form_data_field();
  Init_plamo_form_data_field_array();
  Init_plamo_form_data();
  Init_plamo_middleware();
  Init_plamo_app();
}
