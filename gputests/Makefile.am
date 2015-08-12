CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST = data

if BUILD_GPUTESTS

# This tells automake that we want to build binaries, but they shouldn't be
# installed. For each individual test, add it's binary name here

noinst_PROGRAMS = texture_power_of_2 framebufferobject quad_2texmod texture_copy_blur texture_blur texture_data

if !NUX_OPENGLES_20
noinst_PROGRAMS += arb_programs_limits
endif

# We only have to do this AM_ once to affect all the binaries we build from
# this Makefile
AM_CPPFLAGS = \
  -I$(srcdir) \
  -I$(top_srcdir) \
  -DPREFIX=\""$(prefix)"\" \
  -DLIBDIR=\""$(libdir)"\" \
  -DDATADIR=\""$(datadir)"\" \
  -DG_LOG_DOMAIN=\"NuxGpuTests\" \
  $(GCC_FLAGS) \
  $(NUX_CORE_CFLAGS) \
  $(NUX_GPUTESTS_CFLAGS) \
  $(NUX_CFLAGS) \
  $(MAINTAINER_CFLAGS)

ALL_LIBS = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxGraphics/libnux-graphics-@NUX_API_VERSION@.la \
  $(top_builddir)/Nux/libnux-@NUX_API_VERSION@.la \
  $(NUX_GPUTESTS_LIBS) \
  $(NUX_LIBS)

# This is the individual executable build. For every $exe in noinst_PROGRAMS
# you need a $exe_SOURCES and $exe_LDADD so it builds
texture_power_of_2_SOURCES = texture_power_of_2.cpp
texture_power_of_2_LDADD = $(ALL_LIBS)
framebufferobject_SOURCES = framebufferobject.cpp
framebufferobject_LDADD = $(ALL_LIBS)
quad_2texmod_SOURCES = quad_2texmod.cpp
quad_2texmod_LDADD = $(ALL_LIBS)
texture_copy_blur_SOURCES = texture_copy_blur.cpp
texture_copy_blur_LDADD = $(ALL_LIBS)
texture_blur_SOURCES = texture_blur.cpp
texture_blur_LDADD = $(ALL_LIBS)
texture_data_SOURCES = texture_data.cpp
texture_data_LDADD = $(ALL_LIBS)
arb_programs_limits_SOURCES = arb_programs_limits.cpp
arb_programs_limits_LDADD = $(ALL_LIBS)

# distribute source along to the documentation
sourcegputestsdir = $(pkgdatadir)/gputests
sourcegputests_DATA = $(texture_power_of_2_SOURCES) \
                      $(framebufferobject_SOURCES) \
                      $(quad_2texmod) \
                      $(texture_copy_blur_SOURCES) \
                      $(texture_blur_SOURCES) \
                      $(texture_data_SOURCES) \
                      $(arb_programs_limits_SOURCES)
endif

