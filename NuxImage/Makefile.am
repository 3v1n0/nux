CLEANFILES = 
DISTCLEANFILES =
EXTRA_DIST =

lib_LTLIBRARIES = \
  libnux-image-@NUX_API_VERSION@.la

libnux_image_@NUX_API_VERSION@_la_CPPFLAGS= \
  -I$(srcdir) \
  -I$(top_srcdir) \
  -DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
  -DDATADIR=\""$(datadir)"\" \
  -DG_LOG_DOMAIN=\"NuxImage\" \
  $(GCC_FLAGS) \
  $(NUX_IMAGE_CFLAGS) \
  $(MAINTAINER_CFLAGS)

libnux_image_@NUX_API_VERSION@_la_LIBADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(NUX_IMAGE_LIBS)

libnux_image_@NUX_API_VERSION@_la_LDFLAGS = \
  $(NUX_LT_LDFLAGS)

source_h = \
  $(srcdir)/BitmapFormats.h \
  $(srcdir)/CairoGraphics.h \
  $(srcdir)/ImageSurface.h \
  $(srcdir)/GdkGraphics.h

source_cpp = \
  $(srcdir)/BitmapFormats.cpp \
  $(srcdir)/CairoGraphics.cpp \
  $(srcdir)/ImageSurface.cpp \
  $(srcdir)/GdkGraphics.cpp

libnux_image_@NUX_API_VERSION@_la_SOURCES = \
  $(source_cpp) \
  $(source_h)

nux_imagedir = $(includedir)/Nux-@NUX_API_VERSION@/NuxImage
nux_image_HEADERS = \
  $(source_h)

nux-image-@NUX_API_VERSION@.pc: nux-image.pc
	$(AM_V_GEN) cp -f nux-image.pc nux-image-@NUX_API_VERSION@.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = nux-image-@NUX_API_VERSION@.pc

CLEANFILES += nux-image-@NUX_API_VERSION@.pc
DISTCLEANFILES += nux-image.pc
EXTRA_DIST += nux-image.pc.in
