# In a pure-CMake project this is inserted by configure_package_config_file(),
# but we want to be able to build this file the same from the Autotools, so
# let's manually insert this code.
get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()


set_and_check (WEBSOCKET-SERVER_INCLUDE_DIRS "${PACKAGE_PREFIX_DIR}/include")

# We want to provide an absolute path to the library and we know the
# directory and the base name, but not the suffix, so we use CMake's
# find_library() to pick that up.  Users can override this by configuring
# BSON_LIBRARY themselves.
find_library(WEBSOCKET-SERVER_LIBRARY libwebsocket_server.a PATHS "${PACKAGE_PREFIX_DIR}/lib" NO_DEFAULT_PATH)

set (WEBSOCKET-SERVER_LIBRARIES ${WEBSOCKET-SERVER_LIBRARY})


