require "plamo/version"
require 'ffi'

module Plamo
  extend FFI::Library
  ffi_lib 'plamo'

  enum :plamo_scheme, [
    :http,
    :https,
  ]

  enum :plamo_http_version, [
    :http_09,
    :http_10,
    :http_11,
    :http_20,
  ]

  # PlamoString
  typedef :pointer, :plamo_string
  attach_function :plamo_string_new, [:string], :plamo_string
  attach_function :plamo_string_destroy, [:plamo_string], :void
  attach_function :plamo_string_get_char, [:plamo_string], :string

  # PlamoStringArray
  typedef :pointer, :plamo_string_array
  attach_function :plamo_string_array_new, [], :plamo_string_array
  attach_function :plamo_string_array_destroy, [:plamo_string_array], :void
  callback :plamo_string_array_for_each_callback, [:string], :void
  attach_function :plamo_string_array_for_each, [:plamo_string_array, :plamo_string_array_for_each_callback], :void
  attach_function :plamo_string_array_get_at, [:plamo_string_array, :size_t], :string
  attach_function :plamo_string_array_get_first, [:plamo_string_array], :string
  attach_function :plamo_string_array_get_last, [:plamo_string_array], :string
  attach_function :plamo_string_array_add, [:plamo_string_array, :string], :void
  attach_function :plamo_string_array_remove_at, [:plamo_string_array, :size_t], :bool

  # PlamoByteArray
  typedef :pointer, :plamo_byte_array
  typedef :pointer, :uchar_pointer
  attach_function :plamo_byte_array_new, [:uchar_pointer, :size_t], :plamo_byte_array
  attach_function :plamo_byte_array_destroy, [:plamo_byte_array], :void
  attach_function :plamo_byte_array_get_body, [:plamo_byte_array], :uchar_pointer
  attach_function :plamo_byte_array_get_body_size, [:plamo_byte_array], :size_t

  # PlamoHttpHeader
  typedef :pointer, :plamo_http_header
  attach_function :plamo_http_header_new, [], :plamo_http_header
  attach_function :plamo_http_header_destroy, [:plamo_http_header], :void
  callback :plamo_http_header_for_each_callback, [:string, :plamo_string_array], :void
  attach_function :plamo_http_header_for_each, [:plamo_http_header, :plamo_http_header_for_each_callback], :void
  attach_function :plamo_http_header_get, [:plamo_http_header, :string], :plamo_string_array
  attach_function :plamo_http_header_add, [:plamo_http_header, :string, :string], :void
  attach_function :plamo_http_header_remove, [:plamo_http_header, :string], :bool

  # PlamoHttpQuery
  typedef :pointer, :plamo_http_query
  attach_function :plamo_http_query_new, [], :plamo_http_query
  attach_function :plamo_http_query_destroy, [:plamo_http_query], :void
  attach_function :plamo_http_query_get, [:plamo_http_query, :string], :plamo_string_array
  attach_function :plamo_http_query_add, [:plamo_http_query, :string, :string], :void
  attach_function :plamo_http_query_remove, [:plamo_http_query, :string], :bool

  # PlamoRequest
  class PlamoRequest < FFI::Struct
    layout :scheme, :plamo_scheme,
      :version, :plamo_http_version,
      :method, :plamo_string,
      :path, :plamo_string,
      :query, :plamo_http_query,
      :header, :plamo_http_header,
      :body, :plamo_byte_array
  end

  attach_function :plamo_request_new, [:plamo_scheme, :plamo_http_version, :string, :string, :plamo_http_query, :plamo_http_header, :plamo_byte_array], PlamoRequest.by_ref
  attach_function :plamo_request_destroy, [PlamoRequest.by_ref], :void

  # PlamoResponse
  class PlamoResponse < FFI::Struct
    layout :status_code, :uint,
      :header, :plamo_http_header,
      :body, :plamo_byte_array
  end

  attach_function :plamo_response_new, [], PlamoResponse.by_ref
  attach_function :plamo_response_destroy, [PlamoResponse.by_ref], :void

  # PlamoMiddleware
  class PlamoMiddleware < FFI::Struct
    layout :config, :pointer,
      :callback, :pointer
  end

  callback :plamo_middleware_new_callback, [:pointer, PlamoRequest.by_ref, PlamoResponse.by_ref], :bool
  attach_function :plamo_middleware_new, [:pointer, :plamo_middleware_new_callback], PlamoMiddleware.by_ref
  attach_function :plamo_middleware_destroy, [PlamoMiddleware.by_ref], :void

  # PlamoApp
  typedef :pointer, :plamo_app
  attach_function :plamo_app_new, [], :plamo_app
  attach_function :plamo_app_destroy, [:plamo_app], :void
  attach_function :plamo_app_add_middleware, [:plamo_app, PlamoMiddleware.by_ref], :void
  attach_function :plamo_app_execute, [:plamo_app, PlamoRequest.by_ref], PlamoResponse.by_ref
end
