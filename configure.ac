#
# Based on Clutter's autofoo. Here's how this goes down:
#
# api_version = The soname version "libnux-1.0.so"
# version = The package version "1.2.2"
# 
# Making a point release:
#   - Increase micro_version to the next even number
#   - Increase interface_age to the next even number UNLESS there was an API
#     addition/deprecation, at which point you should set it to 0
#
# After the release:
#   - Increase micro_version to the next odd number
#   - Increase interface_version to the next odd number
# 
m4_define([nux_major_version], [0])
m4_define([nux_minor_version], [9])
m4_define([nux_micro_version], [20])

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
m4_define([nux_interface_age], [4])
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
NUX_LT_LDFLAGS="-version-info $NUX_LT_VERSION"

AC_SUBST(NUX_LT_VERSION)
AC_SUBST(NUX_LT_LDFLAGS)

AC_CONFIG_MACRO_DIR([m4])

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

PKG_CHECK_MODULES(NUX_CORE, glib-2.0 >= 2.25.14 gthread-2.0 sigc++-2.0)
AC_SUBST(NUX_CORE_CFLAGS)
AC_SUBST(NUX_CORE_LIBS)

PKG_CHECK_MODULES(NUX_IMAGE,
                  glib-2.0 >= 2.25.14
                  gdk-pixbuf-2.0
                  cairo >= 1.9.14
                  libpng >= 1.2.44
                  sigc++-2.0
                  )
AC_SUBST(NUX_IMAGE_CFLAGS)
AC_SUBST(NUX_IMAGE_LIBS)

PKG_CHECK_MODULES(NUX_MESH,
                  glib-2.0 >= 2.25.14
                  gdk-pixbuf-2.0
                  glew
                  glewmx
                  sigc++-2.0
                  )
AC_SUBST(NUX_MESH_CFLAGS)
AC_SUBST(NUX_MESH_LIBS)

PKG_CHECK_MODULES(NUX_GRAPHICS,
                  glib-2.0 >= 2.25.14
                  gdk-pixbuf-2.0
                  gl
                  glew
                  glewmx
                  xxf86vm
                  sigc++-2.0
                  )
AC_SUBST(NUX_GRAPHICS_CFLAGS)
AC_SUBST(NUX_GRAPHICS_LIBS)

PKG_CHECK_MODULES(NUX,
                  glib-2.0 >= 2.25.14
                  gdk-pixbuf-2.0
                  gthread-2.0
                  gl
                  glew
                  glewmx
                  sigc++-2.0
                  pango
                  pangocairo
                  libpcre
                  )
AC_SUBST(NUX_CFLAGS)
AC_SUBST(NUX_LIBS)


dnl ===========================================================================

# sets up doxygen
DX_DOXYGEN_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(nux, doxygen.cfg, doc)

AC_ARG_ENABLE([documentation],
              [AC_HELP_STRING([--enable-documentation=@<:@no/yes@:>@],
              [Enable building of documentation @<:@default=no@:>@])],
              [],
              [enable_documentation=no])

AM_CONDITIONAL(BUILD_DOCUMENTATION, [test "x$enable_documentation" = "xyes"])

dnl ==========================================================================

if test "x$GCC" = "xyes"; then
  GCC_FLAGS="-g -Wall"
fi
AC_SUBST(GCC_FLAGS)

# use strict compiler flags only on development releases
m4_define([maintainer_mode_default], [m4_if(m4_eval(nux_micro_version % 2), [1], [yes], [no])])
AC_ARG_ENABLE([maintainer-mode],
              [AC_HELP_STRING([--enable-maintainer-mode=@<:@no/yes@:>@],
                              [Use strict compiler flags @<:@default=no@:>@])],
              [],
              [enable_maintainer_mode=maintainer_mode_default])

MAINTAINER_CFLAGS=""
AS_IF([test "x$enable_maintainer_mode" = "xyes" && test "x$GCC" = "xyes"],
      [
         MAINTAINER_CFLAGS+="-Werror -Wall -Wcast-align -Wno-uninitialized -Wempty-body -Wformat-security -Winit-self"
      ]
)

# this enables lots of useful debugging output in Nux
AC_ARG_ENABLE([debug],
              [AC_HELP_STRING([--enable-debug=@<:@no/yes@:>@],
              [Enable Nux debugging @<:@default=no@:>@])],
              [],
              [enable_debug=no])
AS_IF([test "x$enable_debug" = "xyes"],
      [
         MAINTAINER_CFLAGS+=" -DNUX_DEBUG"
      ]
)

AC_SUBST(MAINTAINER_CFLAGS)

# Enable opengl es 20 code path
AC_ARG_ENABLE([opengles_20],
              [AC_HELP_STRING([--enable-opengles-20=@<:@no/yes@:>@],
              [Enable OpenGL ES 20 code path @<:@default=no@:>@])],
              [],
              [enable_opengles_20=no])
AS_IF([test "x$enable_opengles_20" = "xyes"],
      [
         MAINTAINER_CFLAGS+=" -DNUX_OPENGL_ES_20"
      ]
)

AC_SUBST(MAINTAINER_CFLAGS)

# this enables lots of useful debugging output in Nux
AC_ARG_ENABLE([examples],
              [AC_HELP_STRING([--enable-examples=@<:@no/yes@:>@],
              [Enable building of examples @<:@default=yes@:>@])],
              [],
              [enable_examples=yes])

AM_CONDITIONAL(BUILD_EXAMPLES, [test "x$enable_examples" = "xyes"])
AM_COND_IF([BUILD_EXAMPLES],
      [
          PKG_CHECK_MODULES(NUX_EXAMPLES, x11)
          AC_SUBST(NUX_EXAMPLES_CFLAGS)
          AC_SUBST(NUX_EXAMPLES_LIBS)
      ],
)

# this enables gpu tests
AC_ARG_ENABLE([gputests],
              [AC_HELP_STRING([--enable-gputests=@<:@no/yes@:>@],
              [Enable building of gpu tests @<:@default=yes@:>@])],
              [],
              [enable_gputests=yes])

AM_CONDITIONAL(BUILD_GPUTESTS, [test "x$enable_gputests" = "xyes"])
AM_COND_IF([BUILD_GPUTESTS],
      [
          PKG_CHECK_MODULES(NUX_GPUTESTS, x11)
          AC_SUBST(NUX_GPUTESTS_CFLAGS)
          AC_SUBST(NUX_GPUTESTS_LIBS)
      ],
)

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
  examples/Makefile
  tests/Makefile
  tools/Makefile
  gputests/Makefile
])

AC_OUTPUT

BOLD_GREEN="\033@<:@1;32m"
GREEN="\033@<:@0;32m"
BOLD_WHITE="\033@<:@1;37m"
RESET="\033@<:@0m"

echo ""
echo -e "${BOLD_GREEN} Nux - $VERSION"
echo -e "${RESET}"

echo -e "${GREEN} • Global:${RESET}"
echo -e "        Prefix            : ${BOLD_WHITE}${prefix}"

echo -e "${RESET}"
echo -e "${GREEN} • Compiler Options:${RESET}"
echo -e "        Maintainer CFlags : ${BOLD_WHITE}${MAINTAINER_CFLAGS}${RESET}"
echo -e "        Debug Mode        : ${BOLD_WHITE}${enable_debug}${RESET}"

echo -e "${RESET}"
echo -e "${GREEN} • Documentation:${RESET}"
echo -e "        Build Documenation: ${BOLD_WHITE}${enable_documentation}${RESET}"

echo -e "${RESET}"
echo -e "${GREEN} • Misc Options:${RESET}"
echo -e "        Build Examples    : ${BOLD_WHITE}${enable_examples}${RESET}"
echo -e "        Build Gpu Tests   : ${BOLD_WHITE}${enable_gputests}${RESET}"
echo ""

echo -e "${RESET}"

