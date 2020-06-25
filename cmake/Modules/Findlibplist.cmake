#.rst:
# Findlibplist
# -------
#
# Try to find libplist on a Unix system.
#
# This will define the following variables:
#
# ``libplist_FOUND``
#     True if (the requested version of) libplist is available
# ``libplist_VERSION``
#     The version of libplist
# ``libplist_LIBRARIES``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
# ``libplist_INCLUDE_DIRS``
#     This should be passed to target_include_directories() if the target is not
#     used for linking
# ``libplist_DEFINITIONS``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
#
# If ``libplist_FOUND`` is TRUE, it will also define the following imported target:
#
# ``libplist::libplist``
#     The libplist library
#
# In general we recommend using the imported target, as it is easier to use.
# Bear in mind, however, that if the target is in the link interface of an
# exported library, it must be made available by the package config file.

find_package(PkgConfig)
pkg_check_modules(PKG_libplist QUIET libplist-2.0 libplist)

set(libplist_VERSION ${PKG_libplist_VERSION})
set(libplist_DEFINITIONS ${PKG_libplist_CFLAGS_OTHER})

find_path(libplist_INCLUDE_DIR
    NAMES plist/plist.h
    HINTS ${PKG_libplist_INCLUDE_DIRS}
)

find_library(libplist_LIBRARY
    NAMES plist-2.0 plist
    HINTS ${PKG_libplist_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libplist
    FOUND_VAR libplist_FOUND
    REQUIRED_VARS libplist_LIBRARY
                  libplist_INCLUDE_DIR
    VERSION_VAR libplist_VERSION
)

if (libplist_FOUND AND NOT TARGET libplist::libplist)
    add_library(libplist::libplist UNKNOWN IMPORTED)
    set_target_properties(libplist::libplist PROPERTIES
        IMPORTED_LOCATION "${libplist_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${libplist_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${libplist_INCLUDE_DIR}"
    )
endif()

set(libplist_INCLUDE_DIRS ${libplist_INCLUDE_DIR})
set(libplist_LIBRARIES ${libplist_LIBRARY})

mark_as_advanced(libplist_INCLUDE_DIR)
mark_as_advanced(libplist_LIBRARY)
