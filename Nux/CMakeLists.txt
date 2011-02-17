# The name of our project is "HELLO". CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.

if(UNIX)
#    find_package(GLIB2 REQUIRED)
#    if(GLIB2_FOUND)
#        include_directories(${GLIB2_INCLUDE_DIR})
#    endif(GLIB2_FOUND)
  
  include_directories(/usr/lib/glib-2.0/include)
  include_directories(/usr/include/glib-2.0)
  include_directories(/usr/lib/sigc++-2.0/include)
  include_directories(/usr/include/sigc++-2.0)
endif(UNIX)

include_directories(${InalogicBuild_SOURCE_DIR}
                    ${InalogicBuild_SOURCE_DIR}/NuxCore
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/Math
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/Character
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/FileManager
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/SmartPtr
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/Win32Dialogs
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/TinyXML
                    ${InalogicBuild_SOURCE_DIR}/NuxImage
                    ${InalogicBuild_SOURCE_DIR}/NuxGraphics
                    ${InalogicBuild_SOURCE_DIR}/Nux)

if(WIN32)
    set (SOURCES
		PropertyItem/CheckBoxProperty.cpp
		PropertyItem/CheckBoxProperty.h
		PropertyItem/ColorGradientPropertyItem.cpp
		PropertyItem/ColorGradientPropertyItem.h
		PropertyItem/ColorPreviewPropertyItem.cpp
		PropertyItem/ColorPreviewPropertyItem.h
		PropertyItem/ComboBoxListPropertyItem.cpp
		PropertyItem/ComboBoxListPropertyItem.h
		PropertyItem/ComboBoxPropertyItem.cpp
		PropertyItem/ComboBoxPropertyItem.h
		PropertyItem/DoubleValuatorPropertyItem.cpp
		PropertyItem/DoubleValuatorPropertyItem.h
		PropertyItem/EditTextLinePropertyItem.cpp
		PropertyItem/EditTextLinePropertyItem.h
		PropertyItem/FilePropertyItem.cpp
		PropertyItem/FilePropertyItem.h
		PropertyItem/IntegerValuatorPropertyItem.cpp
		PropertyItem/IntegerValuatorPropertyItem.h
		PropertyItem/Matrix3PreviewPropertyItem.cpp
		PropertyItem/Matrix3PreviewPropertyItem.h
		PropertyItem/Matrix4PreviewPropertyItem.cpp
		PropertyItem/Matrix4PreviewPropertyItem.h
		PropertyItem/RangeValueIntegerPropertyItem.cpp
		PropertyItem/RangeValueIntegerPropertyItem.h
		PropertyItem/RangeValuePropertyItem.cpp
		PropertyItem/RangeValuePropertyItem.h
		PropertyItem/RGBAProperty.cpp
		PropertyItem/RGBAProperty.h
		PropertyItem/RGBProperty.cpp
		PropertyItem/RGBProperty.h
		PropertyItem/SpinBoxDoublePropertyItem.cpp
		PropertyItem/SpinBoxDoublePropertyItem.h
		PropertyItem/SpinBoxPropertyItem.cpp
		PropertyItem/SpinBoxPropertyItem.h
		PropertyItem/SplineCurvePropertyItem.cpp
		PropertyItem/SplineCurvePropertyItem.h
		PropertyItem/Vector3Property.cpp
		PropertyItem/Vector3Property.h
		PropertyItem/Vector3ValuatorPropertyItem.cpp
		PropertyItem/Vector3ValuatorPropertyItem.h
		PropertyItem/Vector4Property.cpp
		PropertyItem/Vector4Property.h

#		TextView/codepages.h
#		TextView/racursor.h
#		TextView/Sequence.cpp
#		TextView/Sequence.h
#		TextView/TextView.cpp
#		TextView/TextView.h
#		TextView/TextViewFile.cpp
#		TextView/TextViewFont.cpp
#		TextView/TextViewInternal.h
#		TextView/TextViewKeyNav.cpp
#		TextView/TextViewMouse.cpp
#		TextView/TextViewPaint.cpp
#		TextView/TextViewScroll.cpp
#		TextView/TextViewSyntax.cpp
#		TextView/TextViewUsp.cpp
#		TextView/TextViewWidget.cpp
#		TextView/TextViewWidget.h
#		TextView/Unicode.c
#		TextView/Unicode.cpp
#		TextView/Unicode.h
		    
		AbstractBackground.cpp
		AbstractBackground.h
		AbstractButton.cpp
		AbstractButton.h
		AbstractComboBox.cpp
		AbstractComboBox.h
		AbstractSeparator.cpp
		AbstractSeparator.h
		AbstractThread.cpp
		AbstractThread.h
		ActionItem.cpp
		ActionItem.h
		AnimatedTextureArea.cpp
		AnimatedTextureArea.h
		PaintLayer.cpp
		PaintLayer.h
		BaseArea.cpp
		BaseArea.h
		BaseObject.cpp
		BaseObject.h
		BaseWindow.cpp
		BaseWindow.h
		BezierCurveControl.cpp
		BezierCurveControl.h
		BezierCurveControl2.cpp
		BezierCurveControl2.h
		Button.cpp
		Button.h
		Camera.cpp
		Camera.h
		CheckBox.cpp
		CheckBox.h
		ClientArea.cpp
		ClientArea.h
		ColorArea.cpp
		ColorArea.h
		ColorEditor.cpp
		ColorEditor.h
		ColorGradient.cpp
		ColorGradient.h
		ColorPickerDialog.cpp
		ColorPickerDialog.h
		ColorPreview.cpp
		ColorPreview.h
		ColorRangeValuator.cpp
		ColorRangeValuator.h
		ComboBoxComplex.cpp
		ComboBoxComplex.h
		ComboBoxSimple.cpp
		ComboBoxSimple.h
		CurveControl.cpp
		CurveControl.h
		Dialog.cpp
		Dialog.h
		DoubleValidator.cpp
		DoubleValidator.h
		EditTextBox.cpp
		EditTextBox.h
		FileSelector.cpp
		FileSelector.h
		FloatingWindow.cpp
		FloatingWindow.h
		FolderTreeItem.cpp
		FolderTreeItem.h
		FunctionGraph.cpp
		FunctionGraph.h
		GfxSimulation.cpp
		GfxSimulation.h
		GroupBox.cpp
		GroupBox.h
		GroupBox2.cpp
		GroupBox2.h
		HexRegExpValidator.cpp
		HexRegExpValidator.h
		Histogram.cpp
		Histogram.h
		HLayout.cpp
		HLayout.h
		HScrollBar.cpp
		HScrollBar.h
		HSeparator.cpp
		HSeparator.h
		HSplitter.cpp
		HSplitter.h
		HToolBar.cpp
		HToolBar.h
		HueRangeValuator.cpp
		HueRangeValuator.h
		IntegerValidator.cpp
		IntegerValidator.h
		InterfaceControl.cpp
		InterfaceControl.h
		KeyboardHandler.cpp
		KeyboardHandler.h
		Layout.cpp
		Layout.h
		ListControl.cpp
		ListControl.h
		Matrix3Editor.cpp
		Matrix3Editor.h
		Matrix3Preview.cpp
		Matrix3Preview.h
		Matrix4Editor.cpp
		Matrix4Editor.h
		Matrix4Preview.cpp
		Matrix4Preview.h
		MenuPage.cpp
		MenuPage.h
		MenuBar.cpp
		MenuBar.h
		MouseAreaCtrl.cpp
		MouseAreaCtrl.h
		MouseHandler.cpp
		MouseHandler.h

		NodeItem.cpp
		NodeItem.h
		NodeNetProtocol.cpp
		NodeNetProtocol.h
		NumericValuator.cpp
		NumericValuator.h
		Nux.cpp
		Nux.h
		NuxGlobalInitializer.cpp
		NuxGlobalInitializer.h
		Painter.cpp
		Painter.h
		Panel.cpp
		Panel.h
		PopUpWindow.cpp
		PopUpWindow.h
		PropertyList.cpp
		PropertyList.h
		RadioButton.cpp
		RadioButton.h
		RadioButtonGroup.cpp
		RadioButtonGroup.h
		RangeValue.cpp
		RangeValue.h
		RangeValueInteger.cpp
		RangeValueInteger.h
		Readme.txt
		RGBValuator.cpp
		RGBValuator.h
		ScrollBar.cpp
		ScrollBar.h
		ScrollView.cpp
		ScrollView.h
		SpinBox.cpp
		SpinBox.h
		SpinBoxDouble.cpp
		SpinBoxDouble.h
		SpinBox_Logic.cpp
		SpinBox_Logic.h
		SplineCurveDialog.cpp
		SplineCurveDialog.h
		SplineCurveEditor.cpp
		SplineCurveEditor.h
		SplineCurvePreview.cpp
		SplineCurvePreview.h
		WindowCompositor.cpp
		WindowCompositor.h
		StaticTextBox.cpp
		StaticTextBox.h
		SystemThread.cpp
		SystemThread.h
		TableCtrl.cpp
		TableCtrl.h
		TableItem.cpp
		TableItem.h
		TabView.cpp
		TabView.h
		TextureArea.cpp
		TextureArea.h
		TimeGraph.cpp
		TimeGraph.h
		TimerManager.cpp
		TimerManager.h
		TimerProc.cpp
		TimerProc.h
		ToolButton.cpp
		ToolButton.h
		TreeControl.cpp
		TreeControl.h
		Utils.cpp
		Utils.h
		Theme.cpp
		Theme.h
		Validator.cpp
		Validator.h
		Valuator.cpp
		Valuator.h
		ValuatorDouble.cpp
		ValuatorDouble.h
		ValuatorInt.cpp
		ValuatorInt.h
		Vector3Valuator.cpp
		Vector3Valuator.h
		Vector3ValuatorDouble.cpp
		Vector3ValuatorDouble.h
		VLayout.cpp
		VLayout.h
		VScrollBar.cpp
		VScrollBar.h
		VSeparator.cpp
		VSeparator.h
		VSplitter.cpp
		VSplitter.h
		VToolBar.cpp
		VToolBar.h
		WidgetMetrics.cpp
		WidgetMetrics.h
		WidgetSmartPointer.cpp
		WidgetSmartPointer.h
		BaseWindow.cpp
		BaseWindow.h
		FloatingWindow.cpp
		FloatingWindow.h
		WindowThread.cpp
		WindowThread.h
    )
elseif(UNIX)
    set (SOURCES
		PropertyItem/CheckBoxProperty.cpp
		PropertyItem/CheckBoxProperty.h
		PropertyItem/ColorGradientPropertyItem.cpp
		PropertyItem/ColorGradientPropertyItem.h
		PropertyItem/ColorPreviewPropertyItem.cpp
		PropertyItem/ColorPreviewPropertyItem.h
		PropertyItem/ComboBoxListPropertyItem.cpp
		PropertyItem/ComboBoxListPropertyItem.h
		PropertyItem/ComboBoxPropertyItem.cpp
		PropertyItem/ComboBoxPropertyItem.h
		PropertyItem/DoubleValuatorPropertyItem.cpp
		PropertyItem/DoubleValuatorPropertyItem.h
		PropertyItem/EditTextLinePropertyItem.cpp
		PropertyItem/EditTextLinePropertyItem.h
		PropertyItem/FilePropertyItem.cpp
		PropertyItem/FilePropertyItem.h
		PropertyItem/IntegerValuatorPropertyItem.cpp
		PropertyItem/IntegerValuatorPropertyItem.h
		PropertyItem/Matrix3PreviewPropertyItem.cpp
		PropertyItem/Matrix3PreviewPropertyItem.h
		PropertyItem/Matrix4PreviewPropertyItem.cpp
		PropertyItem/Matrix4PreviewPropertyItem.h
		PropertyItem/RangeValueIntegerPropertyItem.cpp
		PropertyItem/RangeValueIntegerPropertyItem.h
		PropertyItem/RangeValuePropertyItem.cpp
		PropertyItem/RangeValuePropertyItem.h
		PropertyItem/RGBAProperty.cpp
		PropertyItem/RGBAProperty.h
		PropertyItem/RGBProperty.cpp
		PropertyItem/RGBProperty.h
		PropertyItem/SpinBoxDoublePropertyItem.cpp
		PropertyItem/SpinBoxDoublePropertyItem.h
		PropertyItem/SpinBoxPropertyItem.cpp
		PropertyItem/SpinBoxPropertyItem.h
		PropertyItem/SplineCurvePropertyItem.cpp
		PropertyItem/SplineCurvePropertyItem.h
		PropertyItem/Vector3Property.cpp
		PropertyItem/Vector3Property.h
		PropertyItem/Vector3ValuatorPropertyItem.cpp
		PropertyItem/Vector3ValuatorPropertyItem.h
		PropertyItem/Vector4Property.cpp
		PropertyItem/Vector4Property.h

#		TextView/codepages.h
#		TextView/racursor.h
#		TextView/Sequence.cpp
#		TextView/Sequence.h
#		TextView/TextView.cpp
#		TextView/TextView.h
#		TextView/TextViewFile.cpp
#		TextView/TextViewFont.cpp
#		TextView/TextViewInternal.h
#		TextView/TextViewKeyNav.cpp
#		TextView/TextViewMouse.cpp
#		TextView/TextViewPaint.cpp
#		TextView/TextViewScroll.cpp
#		TextView/TextViewSyntax.cpp
#		TextView/TextViewUsp.cpp
#		TextView/TextViewWidget.cpp
#		TextView/TextViewWidget.h
#		TextView/Unicode.c
#		TextView/Unicode.cpp
#		TextView/Unicode.h


		AbstractBackground.cpp
		AbstractBackground.h
		AbstractButton.cpp
		AbstractButton.h
		AbstractComboBox.cpp
		AbstractComboBox.h
		AbstractSeparator.cpp
		AbstractSeparator.h
		AbstractThread.cpp
		AbstractThread.h
		ActionItem.cpp
		ActionItem.h
		AnimatedTextureArea.cpp
		AnimatedTextureArea.h
		PaintLayer.cpp
		PaintLayer.h
		BaseArea.cpp
		BaseArea.h
		BaseObject.cpp
		BaseObject.h
		BaseWindow.cpp
		BaseWindow.h
		BezierCurveControl.cpp
		BezierCurveControl.h
		BezierCurveControl2.cpp
		BezierCurveControl2.h
		Button.cpp
		Button.h
		Camera.cpp
		Camera.h
		CheckBox.cpp
		CheckBox.h
		ClientArea.cpp
		ClientArea.h
		ColorArea.cpp
		ColorArea.h
		ColorEditor.cpp
		ColorEditor.h
		ColorGradient.cpp
		ColorGradient.h
		ColorPickerDialog.cpp
		ColorPickerDialog.h
		ColorPreview.cpp
		ColorPreview.h
		ColorRangeValuator.cpp
		ColorRangeValuator.h
		ComboBoxComplex.cpp
		ComboBoxComplex.h
		ComboBoxSimple.cpp
		ComboBoxSimple.h
		CurveControl.cpp
		CurveControl.h
		Dialog.cpp
		Dialog.h
		DoubleValidator.cpp
		DoubleValidator.h
		EditTextBox.cpp
		EditTextBox.h
		FileSelector.cpp
		FileSelector.h
		FloatingWindow.cpp
		FloatingWindow.h
		FolderTreeItem.cpp
		FolderTreeItem.h
		FunctionGraph.cpp
		FunctionGraph.h
		GfxSimulation.cpp
		GfxSimulation.h
		GroupBox.cpp
		GroupBox.h
		GroupBox2.cpp
		GroupBox2.h
		HexRegExpValidator.cpp
		HexRegExpValidator.h
		Histogram.cpp
		Histogram.h
		HLayout.cpp
		HLayout.h
		HScrollBar.cpp
		HScrollBar.h
		HSeparator.cpp
		HSeparator.h
		HSplitter.cpp
		HSplitter.h
		HToolBar.cpp
		HToolBar.h
		HueRangeValuator.cpp
		HueRangeValuator.h
		IntegerValidator.cpp
		IntegerValidator.h
		InterfaceControl.cpp
		InterfaceControl.h
		KeyboardHandler.cpp
		KeyboardHandler.h
		Layout.cpp
		Layout.h
		ListControl.cpp
		ListControl.h
		Matrix3Editor.cpp
		Matrix3Editor.h
		Matrix3Preview.cpp
		Matrix3Preview.h
		Matrix4Editor.cpp
		Matrix4Editor.h
		Matrix4Preview.cpp
		Matrix4Preview.h
		MenuPage.cpp
		MenuPage.h
		MenuBar.cpp
		MenuBar.h
		MouseAreaCtrl.cpp
		MouseAreaCtrl.h
		MouseHandler.cpp
		MouseHandler.h

		NodeItem.cpp
		NodeItem.h
		NodeNetProtocol.cpp
		NodeNetProtocol.h
		NumericValuator.cpp
		NumericValuator.h
		Nux.cpp
		Nux.h
		NuxGlobalInitializer.cpp
		NuxGlobalInitializer.h
		Painter.cpp
		Painter.h
		Panel.cpp
		Panel.h
		PopUpWindow.cpp
		PopUpWindow.h
		PropertyList.cpp
		PropertyList.h
		RadioButton.cpp
		RadioButton.h
		RadioButtonGroup.cpp
		RadioButtonGroup.h
		RangeValue.cpp
		RangeValue.h
		RangeValueInteger.cpp
		RangeValueInteger.h
		Readme.txt
		RGBValuator.cpp
		RGBValuator.h
		ScrollBar.cpp
		ScrollBar.h
		ScrollView.cpp
		ScrollView.h
		SpinBox.cpp
		SpinBox.h
		SpinBoxDouble.cpp
		SpinBoxDouble.h
		SpinBox_Logic.cpp
		SpinBox_Logic.h
#		SplineCurveDialog.cpp
#		SplineCurveDialog.h
		SplineCurveEditor.cpp
		SplineCurveEditor.h
		SplineCurvePreview.cpp
		SplineCurvePreview.h
		WindowCompositor.cpp
		WindowCompositor.h
		StaticTextBox.cpp
		StaticTextBox.h
		SystemThread.cpp
		SystemThread.h
		TableCtrl.cpp
		TableCtrl.h
		TableItem.cpp
		TableItem.h
		TabView.cpp
		TabView.h
		TextureArea.cpp
		TextureArea.h
		TimeGraph.cpp
		TimeGraph.h
		TimerManager.cpp
		TimerManager.h
		TimerProc.cpp
		TimerProc.h
		ToolButton.cpp
		ToolButton.h
		TreeControl.cpp
		TreeControl.h
		Utils.cpp
		Utils.h
		Theme.cpp
		Theme.h
		Validator.cpp
		Validator.h
		Valuator.cpp
		Valuator.h
		ValuatorDouble.cpp
		ValuatorDouble.h
		ValuatorInt.cpp
		ValuatorInt.h
		Vector3Valuator.cpp
		Vector3Valuator.h
		Vector3ValuatorDouble.cpp
		Vector3ValuatorDouble.h
		VLayout.cpp
		VLayout.h
		VScrollBar.cpp
		VScrollBar.h
		VSeparator.cpp
		VSeparator.h
		VSplitter.cpp
		VSplitter.h
		VToolBar.cpp
		VToolBar.h
		WidgetMetrics.cpp
		WidgetMetrics.h
		WidgetSmartPointer.cpp
		WidgetSmartPointer.h
		BaseWindow.cpp
		BaseWindow.h
		FloatingWindow.cpp
		FloatingWindow.h
		WindowThread.cpp
		WindowThread.h
    )
endif()


if(WIN32)
    add_library (Nux ${SOURCES}) 
elseif(UNIX)
    add_library (Nux ${SOURCES}) 
endif()



