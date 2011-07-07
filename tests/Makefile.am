if BUILD_TESTS
check_PROGRAMS = \
  test-nux \
  gtest-nux-core

# Please keep alphabetical
test_nux_SOURCES = \
  test-nux.cpp \
  test-timeline.cpp \
  test-object.cpp \
  test-system.cpp

test_nux_CPPFLAGS = \
  -I$(srcdir) \
  -I$(top_srcdir) \
  -DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
  -DDATADIR=\""$(datadir)"\" \
  -DG_LOG_DOMAIN=\"NuxTests\" \
  -DTESTDIR=\""$(top_srcdir)/tests"\" \
  $(GCC_FLAGS) \
  $(NUX_CORE_CFLAGS) \
  $(NUX_EXAMPLES_CFLAGS) \
  $(NUX_CFLAGS) \
  $(MAINTAINER_CFLAGS)

test_nux_LDADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxImage/libnux-image-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxGraphics/libnux-graphics-@NUX_API_VERSION@.la \
  $(top_builddir)/Nux/libnux-@NUX_API_VERSION@.la \
  $(NUX_LIBS)

gtest_nux_core_SOURCES = \
  FileHelpers.cpp \
  test_async_file_writer.cpp \
  test_logger.cpp \
  test_properties.cpp \
  test_rolling_file_appender.cpp

gtest_nux_core_CPPFLAGS = \
  -I$(srcdir) \
  -I$(top_srcdir) \
  -DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
  -DDATADIR=\""$(datadir)"\" \
  -DG_LOG_DOMAIN=\"NuxTests\" \
  -DTESTDIR=\""$(top_srcdir)/tests"\" \
  $(GCC_FLAGS) \
  $(NUX_CORE_CFLAGS) \
  $(NUX_EXAMPLES_CFLAGS) \
  $(NUX_CFLAGS) \
  $(MAINTAINER_CFLAGS)

gtest_nux_core_LDADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(NUX_LIBS)

gtest_nux_core_LDFLAGS = \
  -lpthread -lgtest -lgtest_main -lgmock \
  -lboost_filesystem -lboost_system `pkg-config --libs gio-2.0`


#run make test as part of make check
check-local: test gtest
test:
	@gtester --verbose -k -o=test-nux-results.xml ./test-nux

flat-echo:
	echo NUX_LIBS  $(NUX_LIBS)

gtest: flag-echo gtest-nux-core
	./gtest-nux-core

check-report:
	@gtester -k -o=test-nux-results.xml -k ./test-nux \
		&& ( gtester-report test-nux-results.xml \
				| sed 's/GTester Unit Test Report</>GTester Unit Test Report (normal)</' \
				> test-nux-results.html ) \
		&& gnome-open ./test-nux-results.html

full-report:
		@gtester -k -o=test-nux-results.xml -k -m=slow ./test-nux \
	  && ( gtester-report test-nux-results.xml \
	      | sed 's/>GTester Unit Test Report</>GTester Unit Test Report (normal)</' \
	      > test-nux-results.html )

clean-generic:
	rm -f test-nux-results.xml test-nux-results.html

.PHONY: check-report full-report gtest test

endif

