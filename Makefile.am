SUBDIRS = data NuxCore NuxImage NuxMesh NuxGraphics Nux examples tests

include doxygen-include.am

if BUILD_DOCUMENTATION
all: doxygen-doc
endif

CLEANFILES = $(DX_CLEANFILES)
DISTCLEANFILES =
EXTRA_DIST = autogen.sh $(DX_CONFIG) doc/html

DISTCHECK_CONFIGURE_FLAGS = 
