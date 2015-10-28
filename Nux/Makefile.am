CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST =

lib_LTLIBRARIES = \
  libnux-@NUX_API_VERSION@.la

libnux_@NUX_API_VERSION@_la_CPPFLAGS= \
  -I$(srcdir) \
  -I$(top_srcdir) \
  -DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
  -DDATADIR=\""$(datadir)"\" \
  -DPKGDATADIR=\""$(pkgdatadir)/@NUX_API_VERSION@"\" \
  -DG_LOG_DOMAIN=\"Nux\" \
  $(GCC_FLAGS) \
  $(NUX_CFLAGS) \
  $(IBUS_CFLAGS) \
  $(GEIS_CFLAGS) \
  $(MAINTAINER_CFLAGS) \
  $(COVERAGE_CFLAGS)

libnux_@NUX_API_VERSION@_la_LIBADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxGraphics/libnux-graphics-@NUX_API_VERSION@.la \
  $(NUX_LIBS) \
  $(IBUS_LIBS) \
  $(GEIS_LIBS)

libnux_@NUX_API_VERSION@_la_LDFLAGS = \
  $(NUX_LT_LDFLAGS) \
  $(COVERAGE_LDFLAGS)

source_cpp = \
  AbstractButton.cpp \
  AbstractCheckedButton.cpp \
  AbstractComboBox.cpp \
  AbstractPaintLayer.cpp \
  AbstractThread.cpp \
  Area.cpp \
  BaseWindow.cpp \
  BasicView.cpp \
  Button.cpp \
  CairoWrapper.cpp \
  Canvas.cpp \
  CheckBox.cpp \
  ClientArea.cpp \
  EMMetrics.cpp \
  GridHLayout.cpp \
  HLayout.cpp \
  HSplitter.cpp \
  InputArea.cpp \
  KeyboardHandler.cpp \
  KineticScrolling/AxisDecelerationAnimation.cpp \
  KineticScrolling/KineticAxisScroller.cpp \
  KineticScrolling/KineticScroller.cpp \
  KineticScrolling/KineticScrollingTickSource.cpp \
  KineticScrolling/VelocityCalculator.cpp \
  KineticScrollView.cpp \
  LayeredLayout.cpp \
  Layout.cpp \
  LinearLayout.cpp \
  MainLoopGLib.cpp \
  Nux.cpp \
  NuxGlobalInitializer.cpp \
  Painter.cpp \
  PaintLayer.cpp \
  StaticText.cpp \
  StaticTextBox.cpp \
  SystemThread.cpp \
  TextEntry.cpp \
  TextLoader.cpp \
  TextureArea.cpp \
  Theme.cpp \
  TimerProc.cpp \
  Utils.cpp \
  VLayout.cpp \
  View.cpp \
  VSplitter.cpp \
  WidgetMetrics.cpp \
  WindowCompositor.cpp \
  WindowThread.cpp

if !NUX_MINIMAL
source_cpp += \
  ActionItem.cpp \
  AnimatedTextureArea.cpp \
  ColorEditor.cpp \
  ColorPickerDialog.cpp \
  ColorPreview.cpp \
  Dialog.cpp \
  DoubleValidator.cpp \
  EditTextBox.cpp \
  FileSelector.cpp \
  FloatingWindow.cpp \
  GroupBox.cpp \
  GroupBox2.cpp \
  HexRegExpValidator.cpp \
  HScrollBar.cpp \
  IntegerValidator.cpp \
  MenuPage.cpp \
  MenuBar.cpp \
  MouseAreaCtrl.cpp \
  NumericValuator.cpp \
  Panel.cpp \
  PopUpWindow.cpp \
  InputAreaProximity.cpp \
  RadioButton.cpp \
  RadioButtonGroup.cpp \
  RangeValue.cpp \
  RangeValueInteger.cpp \
  RGBValuator.cpp \
  ScrollBar.cpp \
  ScrollView.cpp \
  SpinBox.cpp \
  SpinBoxDouble.cpp \
  SpinBox_Logic.cpp \
  TabView.cpp \
  Timeline.cpp \
  TimelineEasings.cpp \
  ToggleButton.cpp \
  ToolButton.cpp \
  Validator.cpp \
  VScrollBar.cpp
endif

if USE_X11
source_cpp += \
  InputMethodIBus.cpp \
  XICClient.cpp \
  XIMCallbacks.cpp \
  XIMController.cpp
endif

if HAVE_GEIS
source_cpp += \
  GeisAdapter.cpp \
  Gesture.cpp \
  GestureBroker.cpp \
  GesturesSubscription.cpp
endif

source_h = \
  $(builddir)/Features.h \
  AbstractButton.h \
  AbstractCheckedButton.h \
  AbstractComboBox.h \
  AbstractPaintLayer.h \
  AbstractThread.h \
  Area.h \
  BaseWindow.h \
  BasicView.h \
  Button.h \
  CairoWrapper.h \
  Canvas.h \
  CheckBox.h \
  ClientArea.h \
  EMMetrics.h \
  GridHLayout.h \
  HLayout.h \
  HSplitter.h \
  InputArea.h \
  KeyboardHandler.h \
  KineticScrolling/AxisDecelerationAnimation.h \
  KineticScrolling/KineticAxisScroller.h \
  KineticScrolling/KineticScroller.h \
  KineticScrolling/KineticScrollingEnums.h \
  KineticScrolling/KineticScrollingTickSource.h \
  KineticScrolling/VelocityCalculator.h \
  KineticScrollView.h \
  LayeredLayout.h \
  Layout.h \
  LinearLayout.h \
  MainLoopGLib.h \
  Nux.h \
  NuxGlobalInitializer.h \
  Painter.h \
  PaintLayer.h \
  StaticText.h \
  StaticTextBox.h \
  SystemThread.h \
  TextEntry.h \
  TextLoader.h \
  TextureArea.h \
  Theme.h \
  TimerProc.h \
  Utils.h \
  VLayout.h \
  View.h \
  VSplitter.h \
  WidgetMetrics.h \
  WindowCompositor.h \
  WindowThread.h

if !NUX_MINIMAL
source_h += \
  $(builddir)/ABI.h \
  ActionItem.h \
  AnimatedTextureArea.h \
  ColorEditor.h \
  ColorPickerDialog.h \
  ColorPreview.h \
  Dialog.h \
  DoubleValidator.h \
  EditTextBox.h \
  FileSelector.h \
  FloatingWindow.h \
  GroupBox.h \
  GroupBox2.h \
  HexRegExpValidator.h \
  HScrollBar.h \
  IntegerValidator.h \
  MenuPage.h \
  MenuBar.h \
  MouseAreaCtrl.h \
  NumericValuator.h \
  NuxTimerTickSource.h \
  Panel.h \
  PopUpWindow.h \
  InputAreaProximity.h \
  RadioButton.h \
  RadioButtonGroup.h \
  RangeValue.h \
  RangeValueInteger.h \
  Readme.txt \
  RGBValuator.h \
  ScrollBar.h \
  ScrollView.h \
  SpinBox.h \
  SpinBoxDouble.h \
  SpinBox_Logic.h \
  TabView.h \
  TextEntryComposeSeqs.h \
  Timeline.h \
  TimelineEasings.h \
  ToolButton.h \
  ToggleButton.h \
  Validator.h \
  VScrollBar.h
endif

if USE_X11
source_h += \
  InputMethodIBus.h \
  XICClient.h \
  XIMController.h
endif

if HAVE_GEIS
source_h += \
  GeisAdapter.h \
  Gesture.h \
  GestureBroker.h \
  GesturesSubscription.h
endif

nuxprogramframework_cpp = \
  ProgramFramework/ProgramTemplate.cpp \
  ProgramFramework/TestView.cpp \
  ProgramFramework/TestTextEntry.cpp

nuxprogramframework_h = \
  ProgramFramework/ProgramTemplate.h \
  ProgramFramework/TestView.h \
  ProgramFramework/TestTextEntry.h


libnux_@NUX_API_VERSION@_la_SOURCES = \
  $(source_cpp) \
  $(source_h) \
  $(nuxprogramframework_cpp) \
  $(nuxprogramframework_h)

nuxdir = $(includedir)/Nux-@NUX_API_VERSION@/Nux
nux_HEADERS = $(source_h)

nuxprogramframeworkdir      = $(includedir)/Nux-@NUX_API_VERSION@/Nux/ProgramFramework
nuxprogramframework_HEADERS = $(nuxprogramframework_h)

kineticscrolling_h = \
  KineticScrolling/AxisDecelerationAnimation.h \
  KineticScrolling/KineticAxisScroller.h \
  KineticScrolling/KineticScroller.h \
  KineticScrolling/KineticScrollingEnums.h \
  KineticScrolling/KineticScrollingTickSource.h \
  KineticScrolling/VelocityCalculator.h

kineticscrollingdir      = $(includedir)/Nux-@NUX_API_VERSION@/Nux/KineticScrolling
kineticscrolling_HEADERS = $(kineticscrolling_h)

# pkg-config file
nux-@NUX_API_VERSION@.pc: nux.pc
	$(AM_V_GEN) cp -f nux.pc nux-@NUX_API_VERSION@.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = nux-@NUX_API_VERSION@.pc

unused_src = \
 ComboBoxSimple.cpp \
 ComboBoxSimple.h \
 D2DTextRenderer.cpp \
 D2DTextRenderer.h \
 GridVLayout.cpp \
 GridVLayout.h \
 PangoText.cpp \
 PangoText.h

CLEANFILES += nux-@NUX_API_VERSION@.pc
DISTCLEANFILES += nux.pc
EXTRA_DIST += nux.pc.in ${unused_src}
