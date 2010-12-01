SUBDIRS = data NuxCore NuxImage NuxMesh NuxGraphics Nux examples tests gputests

include doxygen-include.am

if BUILD_DOCUMENTATION
all: doxygen-doc
endif

DISTCLEANFILES = doc/html/* doc/*
EXTRA_DIST = $(DX_CONFIG) autogen.sh COPYING.gpl

