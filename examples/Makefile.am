CLEANFILES = 
DISTCLEANFILES =
EXTRA_DIST =

if BUILD_EXAMPLES

# This tells automake that we want to build binaries, but they shouldn't be
# installed. For each individual example, add it's binary name here
noinst_PROGRAMS = combobox cairo textedit button checkbox tooltip

# We only have to do this AM_ once to affect all the binaries we build from
# this Makefile
AM_CPPFLAGS = \
  -I$(srcdir) \
  -I$(top_srcdir) \
  -DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
  -DDATADIR=\""$(datadir)"\" \
  -DG_LOG_DOMAIN=\"NuxExamples\" \
  $(GCC_FLAGS) \
  $(NUX_CORE_CFLAGS) \
  $(NUX_EXAMPLES_CFLAGS) \
  $(NUX_CFLAGS) \
  $(MAINTAINER_CFLAGS)

ALL_LIBS = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxImage/libnux-image-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxMesh/libnux-mesh-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxGraphics/libnux-graphics-@NUX_API_VERSION@.la \
  $(top_builddir)/Nux/libnux-@NUX_API_VERSION@.la \
  $(NUX_EXAMPLES_LIBS) \
  $(NUX_LIBS)

# This is the individual executable build. For every $exe in noinst_PROGRAMS
# you need a $exe_SOURCES and $exe_LDADD so it builds
combobox_SOURCES = combobox.cpp
combobox_LDADD = $(ALL_LIBS)
cairo_SOURCES = cairo.cpp
cairo_LDADD = $(ALL_LIBS)
textedit_SOURCES = textedit.cpp
textedit_LDADD = $(ALL_LIBS)
button_SOURCES = button.cpp
button_LDADD = $(ALL_LIBS)
checkbox_SOURCES = checkbox.cpp
checkbox_LDADD = $(ALL_LIBS)
tooltip_SOURCES = tooltip.cpp
tooltip_LDADD = $(ALL_LIBS)

# distribute source along to the documentation
sourceexampledir = "$(pkgdatadir)/examples"
sourceexample_DATA = $(combobox_SOURCES) \
                     $(cairo_SOURCES) \
                     $(textedit_SOURCES) \
                     $(button_SOURCES) \
                     $(checkbox_SOURCES) \
                     $(tooltip_SOURCES)
endif

