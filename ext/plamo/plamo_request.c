#include "plamo.h"
#include "wrapper.h"

VALUE rb_cPlamoRequest;

static VALUE sym_http, sym_https, sym_http_2_0, sym_http_1_1, sym_http_1_0, sym_http_0_9;

static void deallocate(Wrapper *wrapper) {
  plamo_request_destroy(wrapper->inner);
  free(wrapper);
}

static VALUE allocate(VALUE klass) {
  return Data_Wrap_Struct(klass, NULL, deallocate, malloc(sizeof(Wrapper)));
}

static VALUE initialize(VALUE self, VALUE rb_scheme, VALUE rb_version, VALUE rb_method, VALUE rb_path, VALUE rb_query, VALUE rb_header, VALUE rb_body) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  const PlamoScheme scheme = sym_http == rb_scheme ? PlamoSchemeHttp : PlamoSchemeHttps;
  const PlamoHttpVersion version = sym_http_2_0 == rb_version ? PlamoHttpVersionHttp20
                                 : sym_http_1_1 == rb_version ? PlamoHttpVersionHttp11
                                 : sym_http_1_0 == rb_version ? PlamoHttpVersionHttp10
                                 : PlamoHttpVersionHttp09;
  PlamoHttpMethod method;
  if (RB_TYPE_P(rb_method, RUBY_T_FIXNUM)) {
    method.defined_http_method = NUM2SIZET(rb_method);
  } else {
    method.undefined_http_method = StringValueCStr(rb_method);
  }
  const char *path = StringValueCStr(rb_path);
  Wrapper *plamo_http_query_wrapper;
  Data_Get_Struct(rb_query, Wrapper, plamo_http_query_wrapper);
  Wrapper *plamo_http_header_wrapper;
  Data_Get_Struct(rb_header, Wrapper, plamo_http_header_wrapper);
  Wrapper *plamo_byte_array_wrapper;
  Data_Get_Struct(rb_body, Wrapper, plamo_byte_array_wrapper);
  wrapper->inner = plamo_request_new(scheme, version, method, path, plamo_http_query_wrapper->inner, plamo_http_header_wrapper->inner, plamo_byte_array_wrapper->inner);
  return self;
}

static VALUE scheme(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  return ((PlamoRequest*)wrapper->inner)->scheme == PlamoSchemeHttp ? sym_http : sym_https;
}

static VALUE version(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  switch (((PlamoRequest*)wrapper->inner)->version) {
    case PlamoHttpVersionHttp20:
      return sym_http_2_0;
    case PlamoHttpVersionHttp11:
      return sym_http_1_1;
    case PlamoHttpVersionHttp10:
      return sym_http_1_0;
    default:
      return sym_http_0_9;
  }
}

static VALUE method(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  PlamoDefinedHttpMethod method = ((PlamoRequest*)wrapper->inner)->method.defined_http_method;
  if (method == PLAMO_HTTP_METHOD_GET) {
    return rb_str_new2("GET");
  } else if (method == PLAMO_HTTP_METHOD_POST) {
    return rb_str_new2("POST");
  } else if (method == PLAMO_HTTP_METHOD_PUT) {
    return rb_str_new2("PUT");
  } else if (method == PLAMO_HTTP_METHOD_DELETE) {
    return rb_str_new2("DELETE");
  } else if (method == PLAMO_HTTP_METHOD_HEAD) {
    return rb_str_new2("HEAD");
  } else if (method == PLAMO_HTTP_METHOD_CONNECT) {
    return rb_str_new2("CONNECT");
  } else if (method == PLAMO_HTTP_METHOD_OPTIONS) {
    return rb_str_new2("OPTIONS");
  } else if (method == PLAMO_HTTP_METHOD_TRACE) {
    return rb_str_new2("TRACE");
  } else if (method == PLAMO_HTTP_METHOD_PATCH) {
    return rb_str_new2("PATCH");
  } else {
    return rb_str_new2(((PlamoRequest*)wrapper->inner)->method.undefined_http_method);
  }
}

static VALUE path(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  return rb_str_new2(plamo_string_get_char(((PlamoRequest*)wrapper->inner)->path));
}

static VALUE query(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  VALUE rb_plamo_http_query = Data_Wrap_Struct(rb_cPlamoHttpQuery, NULL, free, malloc(sizeof(Wrapper)));
  Wrapper *plamo_http_query_wrapper;
  Data_Get_Struct(rb_plamo_http_query, Wrapper, plamo_http_query_wrapper);
  plamo_http_query_wrapper->inner = ((PlamoRequest*)wrapper->inner)->query;
  return rb_plamo_http_query;
}

static VALUE header(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  VALUE rb_plamo_http_header = Data_Wrap_Struct(rb_cPlamoHttpHeader, NULL, free, malloc(sizeof(Wrapper)));
  Wrapper *plamo_http_header_wrapper;
  Data_Get_Struct(rb_plamo_http_header, Wrapper, plamo_http_header_wrapper);
  plamo_http_header_wrapper->inner = ((PlamoRequest*)wrapper->inner)->header;
  return rb_plamo_http_header;
}

static VALUE body(VALUE self) {
  Wrapper *wrapper;
  Data_Get_Struct(self, Wrapper, wrapper);
  VALUE rb_plamo_byte_array = Data_Wrap_Struct(rb_cPlamoByteArray, NULL, free, malloc(sizeof(Wrapper)));
  Wrapper *plamo_byte_array_wrapper;
  Data_Get_Struct(rb_plamo_byte_array, Wrapper, plamo_byte_array_wrapper);
  plamo_byte_array_wrapper->inner = ((PlamoRequest*)wrapper->inner)->body;
  return rb_plamo_byte_array;
}

void Init_plamo_request(void) {
  sym_http = ID2SYM(rb_intern("http"));
  sym_https = ID2SYM(rb_intern("https"));
  sym_http_2_0 = ID2SYM(rb_intern("http_2_0"));
  sym_http_1_1 = ID2SYM(rb_intern("http_1_1"));
  sym_http_1_0 = ID2SYM(rb_intern("http_1_0"));
  sym_http_0_9 = ID2SYM(rb_intern("http_0_9"));

  rb_cPlamoRequest = rb_define_class_under(rb_mPlamo, "Request", rb_cObject);
  rb_define_alloc_func(rb_cPlamoRequest, allocate);
  rb_define_method(rb_cPlamoRequest, "initialize", initialize, 7);
  rb_define_method(rb_cPlamoRequest, "scheme", scheme, 0);
  rb_define_method(rb_cPlamoRequest, "version", version, 0);
  rb_define_method(rb_cPlamoRequest, "method", method, 0);
  rb_define_method(rb_cPlamoRequest, "path", path, 0);
  rb_define_method(rb_cPlamoRequest, "query", query, 0);
  rb_define_method(rb_cPlamoRequest, "header", header, 0);
  rb_define_method(rb_cPlamoRequest, "body", body, 0);
}
