SUBDIRS = data NuxCore NuxImage NuxMesh NuxGraphics Nux examples tests

include doxygen-include.am

if BUILD_DOCUMENTATION
all: doxygen-doc
endif

CLEANFILES = $(DX_CLEANFILES)
DISTCLEANFILES =
EXTRA_DIST = $(DX_CONFIG)

DISTCHECK_CONFIGURE_FLAGS = 
