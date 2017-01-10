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
m4_define([nux_major_version], [4])
m4_define([nux_minor_version], [0])
m4_define([nux_micro_version], [8])

m4_define([nux_api_version], [4.0])
# Increase the number (to the current date) everytime you propose a branch that breaks the API or ABI
# The number format is : year/month/day
# e.g.: december 5th, 2011 is: 20111205
# To make more than one API change in a day, add a number to the date. Like 20111205.xx

m4_define([nux_abi_version], [20140307.0])
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
AC_COPYRIGHT([Copyright 2010-2013 Inalogic Inc.])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([Makefile.am])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([1.10])

NUX_MAJOR_VERSION=nux_major_version
NUX_MINOR_VERSION=nux_minor_version
NUX_MICRO_VERSION=nux_micro_version
NUX_VERSION=nux_version
NUX_API_VERSION=nux_api_version
NUX_ABI_VERSION=nux_abi_version
AC_SUBST(NUX_MAJOR_VERSION)
AC_SUBST(NUX_MINOR_VERSION)
AC_SUBST(NUX_MICRO_VERSION)
AC_SUBST(NUX_VERSION)
AC_SUBST(NUX_API_VERSION)
AC_SUBST(NUX_ABI_VERSION)

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
AM_CXXFLAGS-"$CXXFLAGS -std=c++0x"
AC_PROG_CC_C99
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
GL_PKGS="gl glu glewmx xext x11"
MAINTAINER_CFLAGS=""

# X11 build
AC_ARG_ENABLE([x_support],
             [AC_HELP_STRING([--enable-x-support=@<:@no/yes@:>@],
             [Enable X11 code path @<:@default=yes@:>@])],
             [],
             [enable_x_support=yes])
AS_IF([test "x$enable_x_support" = "xyes"], [
        MAINTAINER_CFLAGS+=" -DUSE_X11"
      ])
AS_IF([test "x$enable_x_support" = "xno"], [
        MAINTAINER_CFLAGS+=" -DNO_X11"
      ])

AM_CONDITIONAL(USE_X11, [test "x$enable_x_support" = "xyes"])

# Enable opengl es 20 code path
AC_ARG_ENABLE([opengles_20],
              [AC_HELP_STRING([--enable-opengles-20=@<:@no/yes@:>@],
              [Enable OpenGL ES 20 code path @<:@default=no@:>@])],
              [],
              [enable_opengles_20=no])
AS_IF([test "x$enable_opengles_20" = "xyes"],
      [
        MAINTAINER_CFLAGS+=" -DNUX_OPENGLES_20"
        GL_PKGS="egl glesv2"
      ]
)
AM_CONDITIONAL(NUX_OPENGLES_20, [test "x$enable_opengles_20" = "xyes"])

# Minimal builds
AC_ARG_ENABLE([minimal_build],
              [AC_HELP_STRING([--enable-minimal-build=@<:@no/yes@:>@],
              [Enable minimal builds @<:@default=no@:>@])],
              [],
              [enable_minimal_build=no])

AS_IF([test "x$enable_minimal_build" = "xyes"], [
        MAINTAINER_CFLAGS+=" -DNUX_MINIMAL"
      ])
AM_CONDITIONAL(NUX_MINIMAL, [test "x$enable_minimal_build" = "xyes"])


AC_SUBST(GL_PKGS)
AC_SUBST(MAINTAINER_CFLAGS)

dnl ===========================================================================

PKG_CHECK_MODULES(NUX_CORE,
                  glib-2.0 >= 2.25.14
                  gthread-2.0
                  sigc++-2.0
                  gio-2.0
                  )
AC_SUBST(NUX_CORE_CFLAGS)
AC_SUBST(NUX_CORE_LIBS)

PKG_CHECK_MODULES(NUX_GRAPHICS,
                  glib-2.0 >= 2.25.14
                  gdk-pixbuf-2.0
                  cairo >= 1.9.14
                  libpng >= 1.2.44
                  $GL_PKGS
                  xxf86vm
                  sigc++-2.0
		              xinerama
                  )
AC_SUBST(NUX_GRAPHICS_CFLAGS)
AC_SUBST(NUX_GRAPHICS_LIBS)

PKG_CHECK_MODULES(NUX,
                  glib-2.0 >= 2.25.14
                  gdk-pixbuf-2.0
                  gthread-2.0
                  $GL_PKGS
                  sigc++-2.0
                  pango
                  pangocairo
                  libpcre
                  )
AC_SUBST(NUX_CFLAGS)
AC_SUBST(NUX_LIBS)

PKG_CHECK_MODULES(UNITY_SUPPORT_TEST,
                  x11
                  $GL_PKGS
                  xcomposite
                  xdamage
                  libpci
                  )
AC_SUBST(UNITY_SUPPORT_TEST_CFLAGS)
AC_SUBST(UNITY_SUPPORT_TEST_LIBS)

PKG_CHECK_MODULES(IBUS,
                  glib-2.0 >= 2.25.14
                  gio-2.0
                  ibus-1.0
                  gobject-2.0
                  )

AC_SUBST(IBUS_CFLAGS)
AC_SUBST(IBUS_LIBS)

dnl *********************************************************
dnl Enable/disable gestures (geis, from Open Input Framework)
dnl *********************************************************

GEIS_PKGS="libgeis"
AC_ARG_ENABLE(gestures,
              AC_HELP_STRING(--disable-gestures, Disables multitouch gestures support (default: auto-detect)),
              [],
              [enable_gestures=auto])


# Check for geis as an optional dependency
AS_IF([test "x$enable_gestures" = "xauto"],
      [
        PKG_CHECK_MODULES(GEIS,
                          [libgeis >= 2.2.10],
                          [have_geis=yes],
                          [have_geis=no])
      ])


AS_IF([test "x$enable_gestures" = "xyes"],
      [
        PKG_CHECK_MODULES(GEIS,
                          [libgeis >= 2.2.10],
                          [have_geis=yes],
                          [
                            AC_MSG_ERROR([libgeis not found!])
                            have_geis=no
                          ])
      ])

AS_IF([test "x$enable_gestures" = "xno"],
      [have_geis=no])

AS_IF([test "x$have_geis" = "xyes"],
      [
        NUX_GESTURES_SUPPORT="NUX_GESTURES_SUPPORT"
        AM_CONDITIONAL(HAVE_GEIS, true)
      ],
      [
        NUX_GESTURES_SUPPORT="NUX_NO_GESTURES_SUPPORT"
        AM_CONDITIONAL(HAVE_GEIS, false)
        GEIS_PKGS=""
      ])

AC_SUBST(NUX_GESTURES_SUPPORT)

AC_SUBST(GEIS_CFLAGS)
AC_SUBST(GEIS_LIBS)
AC_SUBST(GEIS_PKGS)

dnl ************************************
dnl Enable/disable tests
dnl ************************************


AC_ARG_ENABLE([tests],
              [AC_HELP_STRING([--enable-tests=@<:@no/yes@:>@],
              [Enable Nux Testing @<:@default=yes@:>@])],
              [],
              [enable_tests=yes])

AS_IF([test "x$enable_tests" = "xyes"],
      [
        # Check for google gtest
        CHECK_GTEST
        AS_IF([test "x$have_gtest" != "xyes"],
          [AC_MSG_WARN([you are missing google-test, apt-get install google-mock])])

       #Check for libboost

        AC_LANG_PUSH([C++])
        AC_CHECK_HEADER([boost/filesystem.hpp],
                        [have_boost=yes],
                        [AC_MSG_WARN([you are missing boost, apt-get install libboost-dev])])
        AC_LANG_POP([C++])

        #Check for libboost-system

        AC_LANG_PUSH([C++])
        AC_CHECK_HEADER([boost/system/config.hpp],
                        [have_boost_system=yes],
                        [AC_MSG_WARN([you are missing boost-system, apt-get install libboost-system-dev])])
        AC_LANG_POP([C++])

        #Check for libboost-filesystem

        AC_LANG_PUSH([C++])
        AC_CHECK_HEADER([boost/filesystem/fstream.hpp],
                        [have_boost_fs=yes],
                        [AC_MSG_WARN([you are missing boost-filesystem, apt-get install libboost-filesystem-dev])])
        AC_LANG_POP([C++])

        #Check for libxtst
        AC_CHECK_HEADER([X11/extensions/XTest.h],
                       [have_xtest=yes],
                       [AC_MSG_WARN([you are missing libxtst-dev, apt-get install libxtst-dev])])
      ]
)

AS_IF([test "x$have_xtest" = "xyes" &&
       test "x$have_boost" = "xyes" &&
       test "x$have_boost_system" = "xyes" &&
       test "x$have_boost_fs" = "xyes" &&
       test "x$have_gmock" = "xyes" &&
       test "x$have_gtest" = "xyes"],
  [],
  [enable_tests=no]
)

AM_CONDITIONAL(BUILD_TESTS, [test "x$enable_tests" = "xyes"])

AS_IF([test "x$enable_tests" = "xyes" &&
       test "x$enable_x_support" = "xyes"],
  [
    PKG_CHECK_MODULES(NUX_TESTS, x11)
  ],
  []
)

AC_SUBST(NUX_TESTS_CFLAGS)
AC_SUBST(NUX_TESTS_LIBS)

###########################
# gcov coverage reporting
###########################
m4_include([m4/gcov.m4])
AC_TDD_GCOV
AM_CONDITIONAL([HAVE_GCOV], [test "x$ac_cv_check_gcov" = xyes])
AM_CONDITIONAL([HAVE_LCOV], [test "x$ac_cv_check_lcov" = xyes])
AM_CONDITIONAL([HAVE_GCOVR], [test "x$ac_cv_check_gcovr" = xyes])
AC_SUBST(COVERAGE_CFLAGS)
AC_SUBST(COVERAGE_LDFLAGS)

dnl ===========================================================================

# sets up doxygen
m4_include([m4/ac_doxygen.m4])
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
  GCC_FLAGS="-g -Werror -Wall -Wextra -Wno-unused-parameter -std=c++0x"
fi

# gcc flags for building the tests.
GCC_TESTS_FLAGS="-g -O0 -Wall -Wextra -std=c++0x"

# use strict compiler flags only on development releases
AC_ARG_ENABLE([maintainer-mode],
              [AC_HELP_STRING([--enable-maintainer-mode=@<:@no/yes@:>@],
                              [Use strict compiler flags @<:@default=no@:>@])],
              [],
              [enable_maintainer_mode=no])

AS_IF([test "x$enable_maintainer_mode" = "xyes" && test "x$GCC" = "xyes"],
      [
         MAINTAINER_CFLAGS+="-Werror -Wall -Wextra -Wcast-align -Wno-uninitialized -Wempty-body -Wformat-security -Winit-self -Wno-unused-parameter"
      ]
)

# This enables lots of useful debugging output in Nux.
# In debug mode, relax the number of build failures due to -Werror.
AC_ARG_ENABLE([debug],
              [AC_HELP_STRING([--enable-debug=@<:@no/yes@:>@],
              [Enable Nux debugging @<:@default=no@:>@])],
              [],
              [enable_debug=no])
AS_IF([test "x$enable_debug" = "xyes"],
      [
        MAINTAINER_CFLAGS+=" -DNUX_DEBUG"
        GCC_FLAGS="-g -O0 -Wall -Wextra -Wno-unused-parameter -std=c++0x"
      ]
)

AC_SUBST(GCC_FLAGS)
AC_SUBST(GCC_TESTS_FLAGS)
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
  NuxGraphics/Makefile
  NuxGraphics/nux-graphics.pc
  Nux/Makefile
  Nux/nux.pc
  Nux/ABI.h
  Nux/Features.h
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
echo -e "        Prefix             : ${BOLD_WHITE}${prefix}"

echo -e "${RESET}"
echo -e "${GREEN} • Compiler Options:${RESET}"
echo -e "        CFLAGS             : ${BOLD_WHITE}${CFLAGS} ${GCC_FLAGS}${RESET}" 
echo -e "        Maintainer CFlags  : ${BOLD_WHITE}${MAINTAINER_CFLAGS}${RESET}"
echo -e "        Debug Mode         : ${BOLD_WHITE}${enable_debug}${RESET}"

echo -e "${RESET}"
echo -e "${GREEN} • Documentation:${RESET}"
echo -e "        Build Documentation: ${BOLD_WHITE}${enable_documentation}${RESET}"

echo -e "${RESET}"
echo -e "${GREEN} • Misc Options:${RESET}"
echo -e "        Build Examples     : ${BOLD_WHITE}${enable_examples}${RESET}"
echo -e "        Build Gpu Tests    : ${BOLD_WHITE}${enable_gputests}${RESET}"
echo -e "        Build Nux Tests    : ${BOLD_WHITE}${enable_tests}${RESET}"
echo -e "        Coverage Reporting : ${BOLD_WHITE}${use_gcov}${RESET}"
echo -e "        Gestures support   : ${BOLD_WHITE}${have_geis}${RESET}"
echo -e "        X11 build support  : ${BOLD_WHITE}${enable_x_support}${RESET}"
echo -e "        Minimal build      : ${BOLD_WHITE}${enable_minimal_build}${RESET}"
echo ""

echo -e "${RESET}"
