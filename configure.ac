#
# Stolen unashamedly from Clutter. Here's how this goes down:
#
# api_version = The soname version "libnux-1.0.so"
# version = The package version "1.2.2"
# 
# Making a point release:
#   - Increase micro_version to the next even number
#   - Increasde interface_age to the next even number UNLESS there was an API
#     addition/deprecation, at which point you should set it to 0
#
# After the release:
#   - Increase micro_version to the next odd number
#   - Increase interface_version to the next odd number
# 
m4_define([nux_major_version], [0])
m4_define([nux_minor_version], [9])
m4_define([nux_micro_version], [0])

# Bump this when we break ABI
m4_define([nux_api_version], [0.9])

m4_define([nux_version],
          [nux_major_version.nux_minor_version.nux_micro_version])

# increase the interface age by 1 for each release; if the API changes,
# set to 0. interface_age and binary_age are used to create the soname
# of the shared object:
#
#  (<minor> * 100 + <micro>) - <interface_age>
#
# this allows using the same soname for different micro-releases in case
# no API was added or deprecated. for instance:
#
#   nux 1.2.0  -> 100 * 2 + 0  = 200, interface age = 0 -> 200
#   nux 1.2.2  -> 100 * 2 + 2  = 202, interface age = 2 -> 200
#   nux 1.2.4  -> 100 * 2 + 4  = 204, interface age = 4 -> 200
#   [ API addition, deprecation ]
#   nux 1.2.6  -> 100 * 2 + 6  = 206, interface age = 0 -> 206
#   nux 1.2.8  -> 100 * 2 + 8  = 208, interface age = 2 -> 206
#   nux 1.2.10 -> 100 * 2 + 10 = 210, interface age = 4 -> 206
#   ...
#
m4_define([nux_interface_age], [0])
m4_define([nux_binary_age],
          [m4_eval(100 * nux_minor_version + nux_micro_version)])

AC_PREREQ(2.59)

AC_INIT([nux],
        [nux_version],
        [https://bugs.launchpad.net/nux])
AC_COPYRIGHT([Copyright 2010 Inalogic Inc.])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([Makefile.am])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([1.10])

NUX_MAJOR_VERSION=nux_major_version
NUX_MINOR_VERSION=nux_minor_version
NUX_MICRO_VERSION=nux_micro_version
NUX_VERSION=nux_version
NUX_API_VERSION=nux_api_version
AC_SUBST(NUX_MAJOR_VERSION)
AC_SUBST(NUX_MINOR_VERSION)
AC_SUBST(NUX_MICRO_VERSION)
AC_SUBST(NUX_VERSION)
AC_SUBST(NUX_API_VERSION)

m4_define([lt_current],
          [m4_eval(100 * nux_minor_version + nux_micro_version - nux_interface_age)])
m4_define([lt_revision], [nux_interface_age])
m4_define([lt_age], [m4_eval(nux_binary_age - nux_interface_age)])
NUX_LT_CURRENT=lt_current
NUX_LT_REV=lt_revision
NUX_LT_AGE=lt_age
NUX_LT_VERSION="$NUX_LT_CURRENT:$NUX_LT_REV:$NUX_LT_AGE"
NUX_LT_LDFLAG="-version-info $NUX_LT_VERSION -export-dynamic"

AC_SUBST(NUX_LT_VERSION)
AC_SUBST(NUX_LT_LDFLAGS)

dnl ===========================================================================

# Checks for programs
AC_PROG_CXX
AM_PROG_CC_C_O

# require libtool >= 2.2
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

# Checks for header files
AC_HEADER_STDC

# Checks for typedefs, structures and compiler charecteristics
AC_C_CONST

# Checks for library functions
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([memset munmap strcasecmp strdup])

PKG_PROG_PKG_CONFIG

dnl ===========================================================================

PKG_CHECK_MODULES(NUX_CORE, glib-2.0 >= 2.25.14 gthread-2.0)
AC_SUBST(NUX_CORE_CFLAGS)
AC_SUBST(NUX_CORE_LIBS)

PKG_CHECK_MODULES(NUX_IMAGE,
                  glib-2.0 >= 2.25.14
                  cairo >= 1.9.14
                  libpng >= 1.2.44
                  )
AC_SUBST(NUX_IMAGE_CFLAGS)
AC_SUBST(NUX_IMAGE_LIBS)

PKG_CHECK_MODULES(NUX_MESH,
                  glib-2.0 >= 2.25.14
                  glew
                  glewmx
                  )
AC_SUBST(NUX_MESH_CFLAGS)
AC_SUBST(NUX_MESH_LIBS)

PKG_CHECK_MODULES(NUX_GRAPHICS,
                  glib-2.0 >= 2.25.14
                  gl
                  glew
                  glewmx
                  xxf86vm
                  )
AC_SUBST(NUX_GRAPHICS_CFLAGS)
AC_SUBST(NUX_GRAPHICS_LIBS)

# Boost for NUXGraphics
#AC_CHECK_HEADER([boost/shared_ptr.hpp],
#                 [],
#                 [AC_MSG_ERROR([Unable to locate required libboost headers])])

PKG_CHECK_MODULES(NUX,
                  glib-2.0 >= 2.25.14
                  gthread-2.0
                  gl
                  glew
                  glewmx
                  sigc++-2.0
                  )
AC_SUBST(NUX_CFLAGS)
AC_SUBST(NUX_LIBS)


dnl ===========================================================================

if test "x$GCC" = "xyes"; then
  GCC_FLAGS="-g -Wall"
fi
AC_SUBST(GCC_FLAGS)

# use strict compiler flags only on development releases
m4_define([maintainer_flags_default], [m4_if(m4_eval(nux_minor_version % 2), [1], [yes], [no])])
AC_ARG_ENABLE([maintainer-flags],
              [AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                              [Use strict compiler flags @<:@default=no@:>@])],
              [],
              [enable_maintainer_flags=maintainer_flags_default])

MAINTAINER_CFLAGS=""
AS_IF([test "x$enable_maintainer_flags" = "xyes" && test "x$GCC" = "xyes"],
      [
#        MAINTAINER_CFLAGS="-Werror -Wall -Wcast-align -Wno-uninitialized -Wempty-body -Wformat-security -Winit-self"
      ]
)

AC_SUBST(MAINTAINER_CFLAGS)

dnl ===========================================================================

AC_CONFIG_FILES([
  Makefile
  data/Makefile
  NuxCore/Makefile
  NuxCore/nux-core.pc
  NuxImage/Makefile
  NuxImage/nux-image.pc
  NuxMesh/Makefile
  NuxMesh/nux-mesh.pc
  NuxGraphics/Makefile
  NuxGraphics/nux-graphics.pc
  Nux/Makefile
  Nux/nux.pc
  tests/Makefile
])

AC_OUTPUT

echo ""
echo " Nux $VERSION"
echo " =========="
echo ""
echo " Prefix                   : ${prefix}"
echo ""
