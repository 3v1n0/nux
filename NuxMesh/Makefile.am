CLEANFILES = 
DISTCLEANFILES =
EXTRA_DIST =

lib_LTLIBRARIES = \
  libnux-mesh-@NUX_API_VERSION@.la

libnux_mesh_@NUX_API_VERSION@_la_CPPFLAGS= \
  -I$(srcdir) \
  -I$(top_srcdir) \
  -DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
  -DDATADIR=\""$(datadir)"\" \
  -DPKGDATADIR=\""$(pkgdatadir)"\" \
  -DG_LOG_DOMAIN=\"NuxMesh\" \
  $(GCC_FLAGS) \
  $(NUX_MESH_CFLAGS) \
  $(MAINTAINER_CFLAGS)

libnux_mesh_@NUX_API_VERSION@_la_LIBADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxImage/libnux-image-@NUX_API_VERSION@.la \
  $(NUX_MESH_LIBS)

libnux_mesh_@NUX_API_VERSION@_la_LDFLAGS = \
  $(NUX_LT_LDFLAGS)

source_h = \
  $(srcdir)/NWorldObject.h \
  $(srcdir)/NMeshObject.h \
  $(srcdir)/GLRenderableMesh.h \
  $(srcdir)/SceneData.h \
  $(srcdir)/NMeshArchiver_ver_0_0_1.h \
  $(srcdir)/NTextureArchiveManager.h \
  $(srcdir)/TextureArchive_ver_0_0_1.h \
  $(srcdir)/NWorldArchiveManager.h \
  $(srcdir)/N3DWorld.h

source_cpp = \
  $(srcdir)/N3DWorld.cpp \
  $(srcdir)/NMeshArchiver_ver_0_0_1.cpp \
  $(srcdir)/NWorldObject.cpp \
  $(srcdir)/NTextureArchiveManager.cpp \
  $(srcdir)/NWorldArchiveManager.cpp \
  $(srcdir)/NMeshObject.cpp \
  $(srcdir)/GLRenderableMesh.cpp \
  $(srcdir)/SceneData.cpp \
  $(srcdir)/TextureArchive_ver_0_0_1.cpp

libnux_mesh_@NUX_API_VERSION@_la_SOURCES = \
  $(source_cpp) \
  $(source_h)

nux_meshdir = $(includedir)/Nux-@NUX_API_VERSION@/NuxMesh
nux_mesh_HEADERS = \
  $(source_h)

nux-mesh-@NUX_API_VERSION@.pc: nux-mesh.pc
	$(AM_V_GEN) cp -f nux-mesh.pc nux-mesh-@NUX_API_VERSION@.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = nux-mesh-@NUX_API_VERSION@.pc

CLEANFILES += nux-mesh-@NUX_API_VERSION@.pc
DISTCLEANFILES += nux-mesh.pc
EXTRA_DIST += nux-mesh.pc.in
