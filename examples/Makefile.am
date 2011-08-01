CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST =

if BUILD_EXAMPLES

# This tells automake that we want to build binaries, but they shouldn't be
# installed. For each individual example, add it's binary name here

noinst_PROGRAMS = button \
                  cairo \
                  canvas \
                  checkbox \
                  combobox \
                  gridlayout \
                  layeredlayout \
                  moveable_view \
                  pango_combobox \
                  pangotextedit \
                  rotate_texture_area \
                  scrollview \
                  tab_view \
                  textedit \
                  text_entry_focus \
                  timegraph \
                  timeline \
                  tooltip


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
canvas_SOURCES = canvas.cpp
canvas_LDADD = $(ALL_LIBS)
textedit_SOURCES = textedit.cpp
textedit_LDADD = $(ALL_LIBS)
button_SOURCES = button.cpp
button_LDADD = $(ALL_LIBS)
checkbox_SOURCES = checkbox.cpp
checkbox_LDADD = $(ALL_LIBS)
tooltip_SOURCES = tooltip.cpp
tooltip_LDADD = $(ALL_LIBS)
timeline_SOURCES = timeline.cpp
timeline_LDADD = $(ALL_LIBS)
timegraph_SOURCES = timegraph.cpp
timegraph_LDADD = $(ALL_LIBS)
gridlayout_SOURCES = gridlayout.cpp
gridlayout_LDADD = $(ALL_LIBS)
pangotextedit_SOURCES = pangotextedit.cpp
pangotextedit_LDADD = $(ALL_LIBS)
moveable_view_SOURCES = moveable_view.cpp
moveable_view_LDADD = $(ALL_LIBS)
layeredlayout_SOURCES = layeredlayout.cpp
layeredlayout_LDADD = $(ALL_LIBS)
scrollview_SOURCES = scrollview.cpp
scrollview_LDADD = $(ALL_LIBS)
tab_view_SOURCES = tab_view.cpp
tab_view_LDADD = $(ALL_LIBS)
pango_combobox_SOURCES = pango_combobox.cpp
pango_combobox_LDADD = $(ALL_LIBS)
rotate_texture_area_SOURCES = rotate_texture_area.cpp
rotate_texture_area_LDADD = $(ALL_LIBS)
text_entry_focus_SOURCES = text_entry_focus.cpp
text_entry_focus_LDADD = $(ALL_LIBS)

# To distribute source add the source code here
#sourceexampledir = "$(pkgdatadir)/examples"
#sourceexample_DATA = $(combobox_SOURCES) \
#                    

endif



