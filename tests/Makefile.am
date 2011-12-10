if BUILD_TESTS
check_PROGRAMS = \
  test-nux \
  gtest-nux \
  gtest-nux-core \
  test-graphics-display \
  test-empty-window \
  button-xtest \
  mouse-events-test \
  mouse-buttons-test

# Please keep alphabetical
test_nux_SOURCES = \
  test-canvas.cpp \
  test-nux.cpp \
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
  Helpers.h \
  Helpers.cpp \
  test_object.cpp \
  test_main.cpp \
  test_async_file_writer.cpp \
  test_logger.cpp \
  test_cairo_wrapper.cpp \
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
  $(top_builddir)/NuxImage/libnux-image-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxGraphics/libnux-graphics-@NUX_API_VERSION@.la \
  $(top_builddir)/Nux/libnux-@NUX_API_VERSION@.la \
  $(NUX_LIBS)

gtest_nux_core_LDFLAGS = \
  -lpthread -lgtest -lgmock \
  -lboost_filesystem -lboost_system

gtest_nux_SOURCES = \
  test-nux-main.cpp \
  test-nux-statictext.cpp \
  test-nux-windowthread.cpp

gtest_nux_CPPFLAGS = \
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

gtest_nux_LDADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxImage/libnux-image-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxGraphics/libnux-graphics-@NUX_API_VERSION@.la \
  $(top_builddir)/Nux/libnux-@NUX_API_VERSION@.la \
  $(NUX_LIBS)

gtest_nux_LDFLAGS = \
  -lpthread -lgtest -lgmock \
  -lboost_filesystem -lboost_system

TestFlags = -I$(srcdir) \
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

TestLibs = $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxImage/libnux-image-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxGraphics/libnux-graphics-@NUX_API_VERSION@.la \
  $(top_builddir)/Nux/libnux-@NUX_API_VERSION@.la \
  $(NUX_LIBS)


test_graphics_display_SOURCES = test_graphics_display.cpp

test_graphics_display_CPPFLAGS = $(TestFlags)
test_graphics_display_LDADD = $(TestLibs)
test_graphics_display_LDFLAGS = -lpthread

test_empty_window_SOURCES = test_empty_window.cpp

test_empty_window_CPPFLAGS = $(TestFlags)
test_empty_window_LDADD = $(TestLibs)
test_empty_window_LDFLAGS = -lpthread

button_xtest_SOURCES = button-xtest.cpp \
  nux_test_framework.cpp \
  nux_test_framework.h \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

button_xtest_CPPFLAGS = $(TestFlags)
button_xtest_LDADD = $(TestLibs)
button_xtest_LDFLAGS = -lpthread -lXtst

mouse_events_test_SOURCES = mouse-events-test.cpp \
  nux_test_framework.cpp \
  nux_test_framework.h \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h \
  test-view.cpp \
  test-view.h

mouse_events_test_CPPFLAGS = $(TestFlags)
mouse_events_test_LDADD = $(TestLibs)
mouse_events_test_LDFLAGS = -lpthread -lXtst

mouse_buttons_test_SOURCES = mouse-buttons-test.cpp \
  nux_test_framework.cpp \
  nux_test_framework.h \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h \
  test-view.cpp \
  test-view.h

mouse_buttons_test_CPPFLAGS = $(TestFlags)
mouse_buttons_test_LDADD = $(TestLibs)
mouse_buttons_test_LDFLAGS = -lpthread -lXtst

#run make test as part of make check
check-local: test gtest test-apps

test:
	@gtester --verbose -k -o=test-nux-results.xml ./test-nux

gtest: gtest-nux-core gtest-nux
	./gtest-nux-core
	./gtest-nux

test-apps: test-graphics-display test-empty-window button-xtest mouse-events-test mouse-buttons-test
	./test-graphics-display
	./test-empty-window
	./button-xtest
	./mouse-events-test
	./mouse-buttons-test

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

