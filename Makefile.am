SUBDIRS = data NuxCore NuxImage NuxMesh NuxGraphics Nux examples tests

include doxygen-include.am

if BUILD_DOCUMENTATION
all: doxygen-doc
endif

DISTCLEANFILES =
EXTRA_DIST = $(DX_CONFIG) doc/html

DISTCHECK_CONFIGURE_FLAGS = --enable-documentation
