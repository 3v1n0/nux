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
  -DG_LOG_DOMAIN=\"Nux\" \
  $(GCC_FLAGS) \
  $(NUX_CFLAGS) \
  $(MAINTAINER_CFLAGS)

libnux_@NUX_API_VERSION@_la_LIBADD = \
  $(top_builddir)/NuxCore/libnux-core-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxImage/libnux-image-@NUX_API_VERSION@.la \
  $(top_builddir)/NuxGraphics/libnux-graphics-@NUX_API_VERSION@.la \
  $(NUX_LIBS)

libnux_@NUX_API_VERSION@_la_LDFLAGS = \
  $(NUX_LT_LDFLAGS)

source_cpp = \
  $(srcdir)/AbstractButton.cpp \
  $(srcdir)/AbstractCheckedButton.cpp \
  $(srcdir)/AbstractComboBox.cpp \
  $(srcdir)/AbstractPaintLayer.cpp \
  $(srcdir)/AbstractThread.cpp \
  $(srcdir)/ActionItem.cpp \
  $(srcdir)/AnimatedTextureArea.cpp \
  $(srcdir)/PaintLayer.cpp \
  $(srcdir)/InputArea.cpp \
  $(srcdir)/Area.cpp \
  $(srcdir)/BaseWindow.cpp \
  $(srcdir)/Button.cpp \
  $(srcdir)/Canvas.cpp \
  $(srcdir)/CairoWrapper.cpp \
  $(srcdir)/CheckBox.cpp \
  $(srcdir)/ClientArea.cpp \
  $(srcdir)/ColorEditor.cpp \
  $(srcdir)/ColorPickerDialog.cpp \
  $(srcdir)/ColorPreview.cpp \
  $(srcdir)/Dialog.cpp \
  $(srcdir)/DoubleValidator.cpp \
  $(srcdir)/EditTextBox.cpp \
  $(srcdir)/FileSelector.cpp \
  $(srcdir)/FloatingWindow.cpp \
  $(srcdir)/GridHLayout.cpp \
  $(srcdir)/GroupBox.cpp \
  $(srcdir)/GroupBox2.cpp \
  $(srcdir)/HexRegExpValidator.cpp \
  $(srcdir)/HLayout.cpp \
  $(srcdir)/HScrollBar.cpp \
  $(srcdir)/HSplitter.cpp \
  $(srcdir)/IntegerValidator.cpp \
  $(srcdir)/View.cpp \
  $(srcdir)/KeyboardHandler.cpp \
  $(srcdir)/LayeredLayout.cpp \
  $(srcdir)/Layout.cpp \
  $(srcdir)/LinearLayout.cpp \
  $(srcdir)/MenuPage.cpp \
  $(srcdir)/MenuBar.cpp \
  $(srcdir)/MouseAreaCtrl.cpp \
  $(srcdir)/MouseHandler.cpp \
  $(srcdir)/NumericValuator.cpp \
  $(srcdir)/Nux.cpp \
  $(srcdir)/NuxGlobalInitializer.cpp \
  $(srcdir)/Painter.cpp \
  $(srcdir)/Panel.cpp \
  $(srcdir)/PopUpWindow.cpp \
  $(srcdir)/RadioButton.cpp \
  $(srcdir)/RadioButtonGroup.cpp \
  $(srcdir)/RangeValue.cpp \
  $(srcdir)/RangeValueInteger.cpp \
  $(srcdir)/RGBValuator.cpp \
  $(srcdir)/ScrollBar.cpp \
  $(srcdir)/ScrollView.cpp \
  $(srcdir)/SpinBox.cpp \
  $(srcdir)/SpinBoxDouble.cpp \
  $(srcdir)/SpinBox_Logic.cpp \
  $(srcdir)/StaticText.cpp \
  $(srcdir)/StaticTextBox.cpp \
  $(srcdir)/SystemThread.cpp \
  $(srcdir)/TabView.cpp \
  $(srcdir)/TextEntry.cpp \
  $(srcdir)/TextureArea.cpp \
  $(srcdir)/Timeline.cpp \
  $(srcdir)/TimelineEasings.cpp \
  $(srcdir)/TimerProc.cpp \
  $(srcdir)/ToggleButton.cpp \
  $(srcdir)/ToolButton.cpp \
  $(srcdir)/Utils.cpp \
  $(srcdir)/Theme.cpp \
  $(srcdir)/Validator.cpp \
  $(srcdir)/VLayout.cpp \
  $(srcdir)/VScrollBar.cpp \
  $(srcdir)/VSplitter.cpp \
  $(srcdir)/WidgetMetrics.cpp \
  $(srcdir)/WindowCompositor.cpp \
  $(srcdir)/WindowThread.cpp

source_h = \
  $(srcdir)/ABI.h \
  $(srcdir)/AbstractButton.h \
  $(srcdir)/AbstractCheckedButton.h \
  $(srcdir)/AbstractComboBox.h \
  $(srcdir)/AbstractPaintLayer.h \
  $(srcdir)/AbstractThread.h \
  $(srcdir)/ActionItem.h \
  $(srcdir)/AnimatedTextureArea.h \
  $(srcdir)/PaintLayer.h \
  $(srcdir)/InputArea.h \
  $(srcdir)/Area.h \
  $(srcdir)/BaseWindow.h \
  $(srcdir)/Button.h \
  $(srcdir)/Canvas.h \
  $(srcdir)/CairoWrapper.h \
  $(srcdir)/CheckBox.h \
  $(srcdir)/ClientArea.h \
  $(srcdir)/ColorEditor.h \
  $(srcdir)/ColorPickerDialog.h \
  $(srcdir)/ColorPreview.h \
  $(srcdir)/Dialog.h \
  $(srcdir)/DoubleValidator.h \
  $(srcdir)/EditTextBox.h \
  $(srcdir)/FileSelector.h \
  $(srcdir)/FloatingWindow.h \
  $(srcdir)/GridHLayout.h \
  $(srcdir)/GroupBox.h \
  $(srcdir)/GroupBox2.h \
  $(srcdir)/HexRegExpValidator.h \
  $(srcdir)/HLayout.h \
  $(srcdir)/HScrollBar.h \
  $(srcdir)/HSplitter.h \
  $(srcdir)/IntegerValidator.h \
  $(srcdir)/View.h \
  $(srcdir)/KeyboardHandler.h \
  $(srcdir)/LayeredLayout.h \
  $(srcdir)/Layout.h \
  $(srcdir)/LinearLayout.h \
  $(srcdir)/MenuPage.h \
  $(srcdir)/MenuBar.h \
  $(srcdir)/MouseAreaCtrl.h \
  $(srcdir)/MouseHandler.h \
  $(srcdir)/NumericValuator.h \
  $(srcdir)/Nux.h \
  $(srcdir)/NuxGlobalInitializer.h \
  $(srcdir)/Painter.h \
  $(srcdir)/Panel.h \
  $(srcdir)/PopUpWindow.h \
  $(srcdir)/RadioButton.h \
  $(srcdir)/RadioButtonGroup.h \  
  $(srcdir)/RangeValue.h \
  $(srcdir)/RangeValueInteger.h \
  $(srcdir)/Readme.txt \
  $(srcdir)/RGBValuator.h \
  $(srcdir)/ScrollBar.h \
  $(srcdir)/ScrollView.h \
  $(srcdir)/SpinBox.h \
  $(srcdir)/SpinBoxDouble.h \
  $(srcdir)/SpinBox_Logic.h \
  $(srcdir)/StaticText.h \
  $(srcdir)/StaticTextBox.h \
  $(srcdir)/SystemThread.h \
  $(srcdir)/TabView.h \
  $(srcdir)/TextEntry.h \
  $(srcdir)/TextureArea.h \
  $(srcdir)/Timeline.h \
  $(srcdir)/TimelineEasings.h \
  $(srcdir)/TimerProc.h \
  $(srcdir)/ToolButton.h \
  $(srcdir)/ToggleButton.h \
  $(srcdir)/Theme.h \
  $(srcdir)/Utils.h \
  $(srcdir)/Validator.h \
  $(srcdir)/VLayout.h \
  $(srcdir)/VScrollBar.h \
  $(srcdir)/VSplitter.h \
  $(srcdir)/WidgetMetrics.h \
  $(srcdir)/WindowCompositor.h \
  $(srcdir)/WindowThread.h

libnux_@NUX_API_VERSION@_la_SOURCES = \
  $(source_cpp) \
  $(source_h)

nuxdir = $(includedir)/Nux-@NUX_API_VERSION@/Nux
nux_HEADERS = $(source_h)

# pkg-config file
nux-@NUX_API_VERSION@.pc: nux.pc
	$(AM_V_GEN) cp -f nux.pc nux-@NUX_API_VERSION@.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = nux-@NUX_API_VERSION@.pc

CLEANFILES += nux-@NUX_API_VERSION@.pc
DISTCLEANFILES += nux.pc
EXTRA_DIST += nux.pc.in
