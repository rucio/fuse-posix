find_package(PkgConfig REQUIRED)

pkg_check_modules(PC_FUSE QUIET REQUIRED fuse>=2.9)
set(FUSE_DEFINITIONS ${PC_FUSE_CFLAGS_OTHER})

find_path(FUSE_INCLUDE_DIR NAMES fuse.h)
find_library(FUSE_LIBRARY NAMES fuse)

mark_as_advanced(FUSE_INCLUDE_DIR FUSE_LIBRARY)

set(PKG_CONFIG_EXECUTABLE "$ENV{PKG_CONFIG}" CACHE FILEPATH "pkg-config executable")
if(PKG_CONFIG_EXECUTABLE)
    execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --modversion fuse
                    OUTPUT_VARIABLE FUSE_VERSION_STRING
                    ERROR_QUIET
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FUSE
                                  REQUIRED_VARS FUSE_LIBRARY FUSE_INCLUDE_DIR
                                  VERSION_VAR FUSE_VERSION_STRING)


if(FUSE_FOUND)
    set(FUSE_LIBRARIES ${FUSE_LIBRARY})
    set(FUSE_INCLUDE_DIRS ${FUSE_INCLUDE_DIR})
endif()