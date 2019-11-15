#include "plamo.h"

VALUE rb_cPlamoRequest;

static VALUE sym_http, sym_https, sym_http_2_0, sym_http_1_1, sym_http_1_0, sym_http_0_9;

static void deallocate(void *plamo_request) {
  plamo_request_destroy(plamo_request);
}

const rb_data_type_t rb_plamo_request_type = {
  "Request",
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
  return TypedData_Wrap_Struct(klass, &rb_plamo_request_type, NULL);
}

static VALUE initialize(VALUE self, VALUE rb_scheme, VALUE rb_version, VALUE rb_method, VALUE rb_path, VALUE rb_query, VALUE rb_header, VALUE rb_body) {
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
  PlamoHttpQuery *plamo_http_query;
  TypedData_Get_Struct(rb_query, PlamoHttpQuery, &rb_plamo_http_query_type, plamo_http_query);
  PlamoHttpHeader *plamo_http_header;
  TypedData_Get_Struct(rb_header, PlamoHttpHeader, &rb_plamo_http_header_type, plamo_http_header);
  PlamoByteArray *plamo_byte_array;
  TypedData_Get_Struct(rb_body, PlamoByteArray, &rb_plamo_byte_array_type, plamo_byte_array);
  DATA_PTR(self) = plamo_request_new(scheme, version, method, path, plamo_http_query, plamo_http_header, plamo_byte_array);
  return self;
}

static VALUE scheme(VALUE self) {
  PlamoRequest *plamo_request;
  TypedData_Get_Struct(self, PlamoRequest, &rb_plamo_request_type, plamo_request);
  return plamo_request->scheme == PlamoSchemeHttp ? sym_http : sym_https;
}

static VALUE version(VALUE self) {
  PlamoRequest *plamo_request;
  TypedData_Get_Struct(self, PlamoRequest, &rb_plamo_request_type, plamo_request);
  switch (plamo_request->version) {
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
  PlamoRequest *plamo_request;
  TypedData_Get_Struct(self, PlamoRequest, &rb_plamo_request_type, plamo_request);
  PlamoDefinedHttpMethod method = plamo_request->method.defined_http_method;
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
    return rb_str_new2(plamo_request->method.undefined_http_method);
  }
}

static VALUE path(VALUE self) {
  PlamoRequest *plamo_request;
  TypedData_Get_Struct(self, PlamoRequest, &rb_plamo_request_type, plamo_request);
  return rb_str_new2(plamo_string_get_char(plamo_request->path));
}

static VALUE query(VALUE self) {
  PlamoRequest *plamo_request;
  TypedData_Get_Struct(self, PlamoRequest, &rb_plamo_request_type, plamo_request);
  VALUE rb_plamo_http_query = TypedData_Wrap_Struct(rb_cPlamoHttpQuery, &rb_plamo_http_query_type, plamo_request->query);
  return rb_plamo_http_query;
}

static VALUE header(VALUE self) {
  PlamoRequest *plamo_request;
  TypedData_Get_Struct(self, PlamoRequest, &rb_plamo_request_type, plamo_request);
  VALUE rb_plamo_http_header = TypedData_Wrap_Struct(rb_cPlamoHttpHeader, &rb_plamo_http_header_type, plamo_request->header);
  return rb_plamo_http_header;
}

static VALUE body(VALUE self) {
  PlamoRequest *plamo_request;
  TypedData_Get_Struct(self, PlamoRequest, &rb_plamo_request_type, plamo_request);
  VALUE rb_plamo_byte_array = TypedData_Wrap_Struct(rb_cPlamoByteArray, &rb_plamo_byte_array_type, plamo_request->body);
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
