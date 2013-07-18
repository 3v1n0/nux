#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Checks whether the gtest source is available on the system. Allows for
# adjusting the include and source path. Sets have_gtest=yes if the source is
# present. Sets GTEST_CPPFLAGS and GTEST_SOURCE to the preprocessor flags and
# source location respectively.

AC_DEFUN([CHECK_GTEST],
[
  AC_ARG_WITH([gtest-include-path],
              [AS_HELP_STRING([--with-gtest-include-path],
                              [location of the Google test headers])],
                [GTEST_CPPFLAGS="-I$withval"])

  AC_ARG_WITH([gtest-source-path],
              [AS_HELP_STRING([--with-gtest-source-path],
                              [location of the Google test sources, defaults to /usr/src/gmock/gtest])],
              [GTEST_SOURCE="$withval"],
              [GTEST_SOURCE="/usr/src/gmock/gtest"])

  GTEST_CPPFLAGS="$GTEST_CPPFLAGS -I$GTEST_SOURCE -I$GTEST_SOURCE/include"

  AC_LANG_PUSH([C++])

  tmp_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="$CPPFLAGS $GTEST_CPPFLAGS"

  AC_CHECK_HEADER([gtest/gtest.h])

  CPPFLAGS="$tmp_CPPFLAGS"

  AC_LANG_POP

  AS_IF([test -f $GTEST_SOURCE/src/gtest-all.cc && test -f $GTEST_SOURCE/src/gtest_main.cc],
        [have_gtest_source=yes], [have_gtest_source=no])

  AS_IF([test "x$ac_cv_header_gtest_gtest_h" = "xyes" && test "x$have_gtest_source" = "xyes"],
        [have_gtest=yes] [AC_SUBST(GTEST_CPPFLAGS)] [AC_SUBST(GTEST_SOURCE)],
        [have_gtest=no])
]) # CHECK_GTEST


AC_DEFUN([CHECK_GMOCK],
[
  AC_ARG_WITH([gmock-include-path],
              [AS_HELP_STRING([--with-gmock-include-path],
                              [location of the Google mock headers])],
                [GMOCK_CPPFLAGS="-I$withval"])

  AC_ARG_WITH([gmock-source-path],
              [AS_HELP_STRING([--with-gmock-source-path],
                              [location of the Google test mock, defaults to /usr/src/gmock])],
              [GMOCK_SOURCE="$withval"],
              [GMOCK_SOURCE="/usr/src/gmock"])

  GMOCK_CPPFLAGS="$GMOCK_CPPFLAGS -I$GMOCK_SOURCE -I$GMOCK_SOURCE/gtest/include"

  AC_LANG_PUSH([C++])

  tmp_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="$CPPFLAGS $GMOCK_CPPFLAGS"

  AC_CHECK_HEADER([gmock/gmock.h])

  CPPFLAGS="$tmp_CPPFLAGS"

  AC_LANG_POP

  AS_IF([test -f $GMOCK_SOURCE/src/gmock-all.cc && test -f $GMOCK_SOURCE/src/gmock_main.cc],
        [have_gmock_source=yes], [have_gmock_source=no])

  AS_IF([test "x$ac_cv_header_gmock_gmock_h" = "xyes" && test "x$have_gmock_source" = "xyes"],
        [have_gmock=yes] [AC_SUBST(GMOCK_CPPFLAGS)] [AC_SUBST(GMOCK_SOURCE)],
        [have_gmock=no])
]) # CHECK_GMOCK
