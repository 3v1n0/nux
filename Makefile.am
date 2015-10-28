SUBDIRS = data NuxCore NuxGraphics Nux examples gputests tests tools

CXXFLAGS += -fno-permissive

include $(top_srcdir)/Makefile.am.coverage
include doxygen-include.am

if BUILD_DOCUMENTATION
all: doxygen-doc
endif

DISTCLEANFILES = doc/html/* doc/*
EXTRA_DIST = $(DX_CONFIG) autogen.sh COPYING.gpl COPYING.lgpl-v2.1 .bzrignore

check-headless:
	cd tests; $(MAKE) check-headless
