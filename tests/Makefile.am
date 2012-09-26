if BUILD_TESTS

nodist_libgtest_a_SOURCES = $(GTEST_SOURCE)/src/gtest-all.cc

libgtest_a_CPPFLAGS = $(GTEST_CPPFLAGS) -w
libgtest_a_CXXFLAGS = $(GTEST_CXXFLAGS) $(AM_CXXFLAGS)
check_LIBRARIES = libgtest.a

check_PROGRAMS = \
  gtest-nux \
  gtest-nux-slow \
  gtest-nuxcore \
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
  xtest-keynav-directions \
  xtest-text-entry \
  xtest-text-entry-deadkeys \
  xtest-text-entry-compositionkeys \
  xtest-text-entry-logic \
  xtest-focus-on-mouse-enter

gtest_nuxcore_SOURCES = \
  Helpers.h \
  Helpers.cpp \
  $(top_builddir)/NuxCore/ColorPrivate.cpp \
  $(top_builddir)/NuxCore/ColorPrivate.h \
  gtest-nuxcore-animation.cpp \
  gtest-nuxcore-async-file-writer.cpp \
  gtest-nuxcore-color.cpp \
  gtest-nuxcore-colorprivate.cpp \
  gtest-nuxcore-logger.cpp \
  gtest-nuxcore-main.cpp \
  gtest-nuxcore-object.cpp \
  gtest-nuxcore-properties.cpp \
  gtest-nuxcore-rolling-file-appender.cpp

gtest_nuxcore_CPPFLAGS = \
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

gtest_nuxcore_LDADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxGraphics/libnux-graphics-@NUX_API_VERSION@.la \
  $(top_builddir)/Nux/libnux-@NUX_API_VERSION@.la \
  $(NUX_LIBS) \
  libgtest.a

gtest_nuxcore_LDFLAGS = \
  -lpthread -lgmock \
  -lboost_filesystem -lboost_system

##### gtest-nux ######

gtest_nux_SOURCES = \
  FakeGestureEvent.h \
  gtest-nux-axisdecelerationanimation.cpp \
  gtest-nux-emmetrics.cpp \
  gtest-nux-globals.cpp \
  gtest-nux-globals.h \
  gtest-nux-kineticscroller.cpp \
  gtest-nux-inputmethodibus.cpp \
  gtest-nux-velocitycalculator.cpp \
  gtest-nux-main.cpp

if HAVE_GEIS
gtest_nux_SOURCES += \
  geis_mock.h \
	geis_mock.cpp \
  gtest-nux-geisadapter.cpp \
  gtest-nux-gesturebroker.cpp
endif

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
  $(MAINTAINER_CFLAGS) \
  $(IBUS_CFLAGS)

gtest_nux_LDADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxGraphics/libnux-graphics-@NUX_API_VERSION@.la \
  $(top_builddir)/Nux/libnux-@NUX_API_VERSION@.la \
  $(NUX_LIBS) \
  libgtest.a

gtest_nux_LDFLAGS = \
  -lpthread -lgmock \
  -lboost_filesystem -lboost_system

##### gtest-nux-slow ######

gtest_nux_slow_SOURCES = \
  gtest-nux-area.cpp \
  gtest-nux-cairo-wrapper.cpp \
  gtest-nux-input-area.cpp \
  gtest-nux-main.cpp \
  gtest-nux-statictext.cpp \
  gtest-nux-scrollview.cpp \
  gtest-nux-textentry.cpp \
  gtest-nux-utils.h \
  gtest-nux-view.cpp \
  gtest-nux-windowcompositor.cpp \
  gtest-nux-windowthread.cpp

gtest_nux_slow_CPPFLAGS = \
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
  $(MAINTAINER_CFLAGS) \
  $(IBUS_CFLAGS)

gtest_nux_slow_LDADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxGraphics/libnux-graphics-@NUX_API_VERSION@.la \
  $(top_builddir)/Nux/libnux-@NUX_API_VERSION@.la \
  $(NUX_LIBS) \
  libgtest.a

gtest_nux_slow_LDFLAGS = \
  -lpthread -lgmock \
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
  $(MAINTAINER_CFLAGS) \
  $(IBUS_CFLAGS)

TestLibs = $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
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

xtest_focus_on_mouse_enter_SOURCES = xtest-focus-on-mouse-enter.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_focus_on_mouse_enter_CPPFLAGS = $(TestFlags)
xtest_focus_on_mouse_enter_LDADD = $(TestLibs)
xtest_focus_on_mouse_enter_LDFLAGS = -lpthread -lXtst

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

xtest_text_entry_SOURCES = xtest-text-entry.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_text_entry_CPPFLAGS = $(TestFlags)
xtest_text_entry_LDADD = $(TestLibs)
xtest_text_entry_LDFLAGS = -lpthread -lXtst

xtest_text_entry_logic_SOURCES = xtest-text-entry-logic.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_text_entry_logic_CPPFLAGS = $(TestFlags)
xtest_text_entry_logic_LDADD = $(TestLibs)
xtest_text_entry_logic_LDFLAGS = -lpthread -lXtst

xtest_text_entry_deadkeys_SOURCES = xtest-text-entry-deadkeys.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_text_entry_deadkeys_CPPFLAGS = $(TestFlags)
xtest_text_entry_deadkeys_LDADD = $(TestLibs)
xtest_text_entry_deadkeys_LDFLAGS = -lpthread -lXtst

xtest_text_entry_compositionkeys_SOURCES = xtest-text-entry-compositionkeys.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_text_entry_compositionkeys_CPPFLAGS = $(TestFlags)
xtest_text_entry_compositionkeys_LDADD = $(TestLibs)
xtest_text_entry_compositionkeys_LDFLAGS = -lpthread -lXtst

#run make test as part of make check
check-local: test gtest test-apps

test:
	@gtester --verbose -k -o=gtest-nux-results.xml ./gtest-nux

gtest: gtest-nuxcore gtest-nux
	./gtest-nuxcore
	./gtest-nux

check-headless: gtest-nuxcore
	@gtester --verbose -k -o=test-nux-results.xml ./gtest-nuxcore

test-apps: test-graphics-display test-empty-window xtest-text-entry xtest-button xtest-mouse-events xtest-mouse-buttons xtest-hgrid-key-navigation xtest-hlayout-key-navigation xtest-vlayout-key-navigation xtest-scrollbar xtest-focus-on-mouse-down xtest-focus-on-mouse-enter xtest-keynav-directions xtest-text-entry-logic xtest-text-entry-deadkeys xtest-text-entry-compositionkeys
	./test-graphics-display
	./test-empty-window
	./xtest-text-entry
	./xtest-button
	./xtest-mouse-events
	./xtest-mouse-buttons
	./xtest-hgrid-key-navigation
	./xtest-hlayout-key-navigation
	./xtest-vlayout-key-navigation
	./xtest-scrollbar
	./xtest-focus-on-mouse-down
	./xtest-focus-on-mouse-enter
	./xtest-keynav-directions
	./xtest-text-entry-logic
	./xtest-text-entry-deadkeys
	./xtest-text-entry-compositionkeys
  

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

