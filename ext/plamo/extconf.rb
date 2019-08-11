require "mkmf"

unless find_header("libplamo.h")
  abort "libplamo is missing. please install libplamo"
end

unless find_library("plamo", "plamo_string_new")
  abort "libplamo is missing. please install libplamo"
end

create_makefile("plamo/plamo")
