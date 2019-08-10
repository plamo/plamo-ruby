#include "plamo.h"

VALUE rb_mPlamo;

void
Init_plamo(void)
{
  rb_mPlamo = rb_define_module("Plamo");
}
