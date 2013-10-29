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
  AC_LANG_PUSH([C++])
  PATH_SEPARATOR=:

  AC_ARG_VAR([GMOCK_PREFIX],
             [Path to Google Mock sources.])
  AC_ARG_VAR([GMOCK_INCLUDES],
             [Path to Google Mock headers.])
  AC_ARG_VAR([GMOCK_CPPFLAGS],
             [C preprocessor flags for Google Mock.])

  gmock_prefix=/usr/src/gmock
  gmock_includes=$gmock_prefix/gtest/include
  AC_ARG_WITH([gmock-prefix],[
    AS_HELP_STRING([--with-gmock-prefix],
                   [Prefix where Google Mock is installed (default $gmock_prefix)])
  ],[
    gmock_prefix=$withval
  ])

  AC_ARG_WITH([gmock-includes],[
    AS_HELP_STRING([--with-gmock-includes],
                   [Search path for Google Mock headers (default none)])
  ],[
    gmock_includes=$withval
  ])

  AS_IF([test x$gmock_includes != x],
        [GMOCK_CPPFLAGS=-I$gmock_includes],
        [GMOCK_CPPFLAGS=])
  tmp_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="$CPPFLAGS $GMOCK_CPPFLAGS"
  AC_CHECK_HEADER([gmock/gmock.h],
                  [have_gmock=yes],
                  [have_gmock=no],
                  [-])
  CPPFLAGS="$tmp_CPPFLAGS"
  AS_IF([test x$have_gmock = xyes],[
    AC_CHECK_FILES([$gmock_prefix/src/gmock-all.cc],
                   [have_gmock=yes],
                   [have_gmock=no])

    gtest_include_path=/usr/include
    gtest_prefix_path=/usr/src/gtest
    AS_IF([test x$have_gmock = xyes],[
      AC_SUBST([GMOCK_CPPFLAGS], "$GMOCK_CPPFLAGS -I$gmock_prefix")
      AC_SUBST([GMOCK_PREFIX], [$gmock_prefix])
      gtest_prefix_path="$GMOCK_PREFIX/gtest$PATH_SEPARATOR$gtest_prefix_path"
      gtest_include_path="$GMOCK_PREFIX/gtest/include$PATH_SEPARATOR$gtest_include_path"
    ])

    AC_ARG_VAR([GTEST_PREFIX],
               [Path to Google Test sources.])
    AC_ARG_VAR([GTEST_CPPFLAGS],
               [C preprocessor flags for Google Test.])

    ax_save_IFS="$IFS"; IFS=$PATH_SEPARATOR
    for incdir in $gtest_include_path; do
      IFS=$ax_save_IFS
      tmp_CPPFLAGS="$CPPFLAGS"
      CPPFLAGS="$CPPFLAGS -I$incdir"
      AC_CHECK_HEADER([gtest/gtest.h],
                      [have_gtest=yes],
                      [have_gtest=no])
      CPPFLAGS="$tmp_CPPFLAGS"
      AS_IF([test x$have_gtest = xyes],[
        GTEST_CPPFLAGS=-I$incdir
        break
      ])
    done
    IFS=$ax_save_IFS

    AS_IF([test x$have_gtest = xyes],[
      ax_save_IFS="$IFS"; IFS=$PATH_SEPARATOR
      for ax_prefix in $gtest_prefix_path; do
        IFS=$ax_save_IFS
        AC_CHECK_FILES([$ax_prefix/src/gtest-all.cc],
                       [have_gtest=yes],
                       [have_gtest=no])
        AS_IF([test x$have_gtest = xyes],[
          AC_SUBST([GTEST_CPPFLAGS], "$GTEST_CPPFLAGS -I$ax_prefix")
          AC_SUBST([GTEST_PREFIX],[$ax_prefix])
          break
        ])
      done
      IFS=$ax_save_IFS
    ])

  ])
]) # CHECK_GTEST

