if BUILD_TESTS
check_PROGRAMS = \
  test-nux \
  gtest-nux \
  gtest-nux-core \
  test-graphics-display \
  test-empty-window \
  xtest-button \
  xtest-mouse-events \
  xtest-mouse-buttons \
  xtest-hgrid-key-navigation \
  xtest-hlayout-key-navigation \
  xtest-vlayout-key-navigation \
  xtest-scrollbar \
  xtest-focus-on-mouse-down \
  xtest-keynav-directions

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
  gtest-nuxcore-color.cpp \
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
  test-nux-metrics.cpp \
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

xtest_button_SOURCES = xtest-button.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_button_CPPFLAGS = $(TestFlags)
xtest_button_LDADD = $(TestLibs)
xtest_button_LDFLAGS = -lpthread -lXtst

xtest_mouse_events_SOURCES = xtest-mouse-events.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_mouse_events_CPPFLAGS = $(TestFlags)
xtest_mouse_events_LDADD = $(TestLibs)
xtest_mouse_events_LDFLAGS = -lpthread -lXtst

xtest_mouse_buttons_SOURCES = xtest-mouse-buttons.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_mouse_buttons_CPPFLAGS = $(TestFlags)
xtest_mouse_buttons_LDADD = $(TestLibs)
xtest_mouse_buttons_LDFLAGS = -lpthread -lXtst

xtest_hgrid_key_navigation_SOURCES = xtest-hgrid-key-navigation.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_hgrid_key_navigation_CPPFLAGS = $(TestFlags)
xtest_hgrid_key_navigation_LDADD = $(TestLibs)
xtest_hgrid_key_navigation_LDFLAGS = -lpthread -lXtst

xtest_hlayout_key_navigation_SOURCES = xtest-hlayout-key-navigation.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_hlayout_key_navigation_CPPFLAGS = $(TestFlags)
xtest_hlayout_key_navigation_LDADD = $(TestLibs)
xtest_hlayout_key_navigation_LDFLAGS = -lpthread -lXtst

xtest_vlayout_key_navigation_SOURCES = xtest-vlayout-key-navigation.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_vlayout_key_navigation_CPPFLAGS = $(TestFlags)
xtest_vlayout_key_navigation_LDADD = $(TestLibs)
xtest_vlayout_key_navigation_LDFLAGS = -lpthread -lXtst

xtest_focus_on_mouse_down_SOURCES = xtest-focus-on-mouse-down.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_focus_on_mouse_down_CPPFLAGS = $(TestFlags)
xtest_focus_on_mouse_down_LDADD = $(TestLibs)
xtest_focus_on_mouse_down_LDFLAGS = -lpthread -lXtst

xtest_scrollbar_SOURCES = xtest-scrollbar.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h \
  test-scrollview.cpp \
  test-scrollview.h

xtest_scrollbar_CPPFLAGS = $(TestFlags)
xtest_scrollbar_LDADD = $(TestLibs)
xtest_scrollbar_LDFLAGS = -lpthread -lXtst

xtest_keynav_directions_SOURCES = xtest-keynav-directions.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_keynav_directions_CPPFLAGS = $(TestFlags)
xtest_keynav_directions_LDADD = $(TestLibs)
xtest_keynav_directions_LDFLAGS = -lpthread -lXtst


#run make test as part of make check
check-local: test gtest test-apps

test:
	@gtester --verbose -k -o=test-nux-results.xml ./test-nux

gtest: gtest-nux-core gtest-nux
	./gtest-nux-core
	./gtest-nux

check-headless: gtest-nux-core
	@gtester --verbose -k -o=test-nux-results.xml ./gtest-nux-core

test-apps: test-graphics-display test-empty-window xtest-button xtest-mouse-events xtest-mouse-buttons xtest-hgrid-key-navigation xtest-hlayout-key-navigation xtest-vlayout-key-navigation xtest-scrollbar xtest-focus-on-mouse-down xtest-keynav-directions
	./test-graphics-display
	./test-empty-window
	./xtest-button
	./xtest-mouse-events
	./xtest-mouse-buttons
	./xtest-hgrid-key-navigation
	./xtest-hlayout-key-navigation
	./xtest-vlayout-key-navigation
	./xtest-scrollbar
	./xtest-focus-on-mouse-down
	./xtest-keynav-directions

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

.PHONY: check check-report full-report gtest test check-headless

endif

