SUBDIRS = data NuxCore NuxImage NuxGraphics Nux examples gputests tests tools

CXXFLAGS += -fno-permissive

include doxygen-include.am

if BUILD_DOCUMENTATION
all: doxygen-doc
endif

DISTCLEANFILES = doc/html/* doc/*
EXTRA_DIST = $(DX_CONFIG) autogen.sh COPYING.gpl .bzrignore

check-headless:
	cd tests; $(MAKE) check-headless
