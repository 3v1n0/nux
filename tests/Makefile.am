if BUILD_TESTS

nodist_libgtest_a_SOURCES = \
 $(GMOCK_PREFIX)/src/gmock-all.cc \
 $(GTEST_PREFIX)/src/gtest-all.cc

libgtest_a_CPPFLAGS = $(GMOCK_CPPFLAGS) $(GTEST_CPPFLAGS) -w
check_LIBRARIES = libgtest.a

check_PROGRAMS = \
  gtest-nux \
  gtest-nux-slow \
  gtest-nuxcore \
  gtest-nuxgraphics \
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
  xtest-text-entry-xim \
  xtest-focus-on-mouse-enter

TestFlags = -I$(srcdir) \
            -I$(top_srcdir) \
            -DPREFIX=\""$(prefix)"\" \
            -DLIBDIR=\""$(libdir)"\" \
            -DDATADIR=\""$(datadir)"\" \
            -DG_LOG_DOMAIN=\"NuxTests\" \
            -DTESTDIR=\""$(top_srcdir)/tests"\" \
            $(GCC_TESTS_FLAGS) \
            $(NUX_CORE_CFLAGS) \
            $(NUX_EXAMPLES_CFLAGS) \
            $(NUX_CFLAGS) \
            $(NUX_TESTS_CFLAGS) \
            $(MAINTAINER_CFLAGS) \
            $(IBUS_CFLAGS) \
            $(GEIS_CFLAGS)

TestLibs = $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
           $(top_builddir)/NuxGraphics/libnux-graphics-@NUX_API_VERSION@.la \
           $(top_builddir)/Nux/libnux-@NUX_API_VERSION@.la \
           $(NUX_TESTS_LIBS) \
           $(NUX_LIBS)

GTestFlags = $(TestFlags) \
             $(GMOCK_CPPFLAGS) \
             $(GTEST_CPPFLAGS)

GTestLibs = $(TestLibs) \
            libgtest.a

GTestLDFlags = -lboost_system \
               -lboost_filesystem

##### gtest-nuxcore ######

gtest_nuxcore_SOURCES = \
  Helpers.h \
  Helpers.cpp \
  $(top_srcdir)/NuxCore/ColorPrivate.cpp \
  $(top_srcdir)/NuxCore/ColorPrivate.h \
  gtest-nuxcore-animation.cpp \
  gtest-nuxcore-async-file-writer.cpp \
  gtest-nuxcore-color.cpp \
  gtest-nuxcore-colorprivate.cpp \
  gtest-nuxcore-logger.cpp \
  gtest-nuxcore-main.cpp \
  gtest-nuxcore-object.cpp \
  gtest-nuxcore-properties.cpp \
  gtest-nuxcore-rolling-file-appender.cpp

gtest_nuxcore_CPPFLAGS = $(GTestFlags)
gtest_nuxcore_LDADD = $(GTestLibs)
gtest_nuxcore_LDFLAGS = $(GTestLDFlags)

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

gtest_nux_CPPFLAGS = $(GTestFlags)
gtest_nux_LDADD = $(GTestLibs)

##### gtest-nux-slow ######

gtest_nux_slow_SOURCES = \
  gtest-nux-area.cpp \
  gtest-nux-cairo-wrapper.cpp \
  gtest-nux-input-area.cpp \
  gtest-nux-main.cpp \
  gtest-nux-inputarea-proximity.cpp \
  gtest-nux-statictext.cpp \
  gtest-nux-scrollview.cpp \
  gtest-nux-textentry.cpp \
  gtest-nux-utils.h \
  gtest-nux-view.cpp \
  gtest-nux-windowcompositor.cpp \
  gtest-nux-windowthread.cpp

gtest_nux_slow_CPPFLAGS = $(GTestFlags)
gtest_nux_slow_LDADD = $(GTestLibs)
gtest_nux_slow_LDFLAGS = $(GTestLDFlags)

##### gtest-nuxgraphics ######

gtest_nuxgraphics_SOURCES = \
  gtest-nuxgraphics-main.cpp \
  gtest-nuxgraphics-texture.cpp \
  gtest-nuxgraphics-graphic-display.cpp

gtest_nuxgraphics_CPPFLAGS = $(GTestFlags)
gtest_nuxgraphics_LDADD = $(GTestLibs)


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
xtest_text_entry_LDFLAGS = -lpthread -lXtst -libus-1.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0

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

xtest_text_entry_xim_SOURCES = xtest-text-entry-xim.cpp \
  nux_automated_test_framework.cpp \
  nux_automated_test_framework.h

xtest_text_entry_xim_CPPFLAGS = $(TestFlags)
xtest_text_entry_xim_LDADD = $(TestLibs)
xtest_text_entry_xim_LDFLAGS = -lpthread -lXtst

#run make test as part of make check
check-local: test gtest test-apps

test:
	@gtester --verbose -k -o=gtest-nux-results.xml ./gtest-nux

gtest-build: gtest-nuxcore gtest-nuxgraphics gtest-nux gtest-nux-slow

gtest: gtest-build
	./gtest-nuxcore
	./gtest-nuxgraphics
	./gtest-nux
	./gtest-nux-slow

CHECK_GTEST_OPTIONS =

if NUX_OPENGLES_20
# Disable EmbeddedContext tests when OpenGLES is enabled, they tend to crash
CHECK_GTEST_OPTIONS = --gtest_filter=-EmbeddedContext*
endif # NUX_OPENGLES_20

check-headless: gtest-nuxcore gtest-nuxgraphics gtest-nux gtest-nux-slow
	@./gtest-nuxcore --gtest_output=xml:./test-nux-core-results.xml $(CHECK_GTEST_OPTIONS)
	@./dummy-xorg-test-runner.sh ./gtest-nuxgraphics --gtest_output=xml:./test-nux-graphics-results.xml $(CHECK_GTEST_OPTIONS)
	@./dummy-xorg-test-runner.sh ./gtest-nux --gtest_output=xml:./test-nux-results.xml $(CHECK_GTEST_OPTIONS)
	@./dummy-xorg-test-runner.sh ./gtest-nux-slow --gtest_output=xml:./test-nux-slow-results.xml $(CHECK_GTEST_OPTIONS)

check: gtest-nuxcore gtest-nuxgraphics gtest-nux gtest-nux-slow
	@./gtest-nuxcore --gtest_output=xml:./test-nux-core-results.xml
	@./gtest-nuxgraphics --gtest_output=xml:./test-nux-graphics-results.xml
	@./gtest-nux --gtest_output=xml:./test-nux-results.xml
	@./gtest-nux-slow --gtest_output=xml:./test-nux-slow-results.xml


test-apps: test-graphics-display test-empty-window xtest-text-entry-xim xtest-text-entry xtest-button xtest-mouse-events xtest-mouse-buttons xtest-hgrid-key-navigation xtest-hlayout-key-navigation xtest-vlayout-key-navigation xtest-scrollbar xtest-focus-on-mouse-down xtest-focus-on-mouse-enter xtest-keynav-directions xtest-text-entry-logic xtest-text-entry-deadkeys xtest-text-entry-compositionkeys
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

CLEANFILES = \
	test-nux-results.xml test-nux-results.html \
	gtest-nux-results.xml

.PHONY: check check-report full-report gtest test check-headless

endif

unused_src = \
 manual-tests/linaro-reference-fbo.txt \
 manual-tests/text-entry.txt \
 test-canvas.cpp \
 test-nux.cpp \
 test-system.cpp \
 test-timeline.cpp \
 xim-test-commands.txt

EXTRA_DIST =  Readme.txt ${unused_src}
