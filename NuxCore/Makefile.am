CLEANFILES = 
DISTCLEANFILES =
EXTRA_DIST =

AM_CPPFLAGS = \
  -I$(srcdir) \
  -I$(top_srcdir) \
  -DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
  -DDATADIR=\""$(datadir)"\" \
  -DG_LOG_DOMAIN=\"NuxCore\"

lib_LTLIBRARIES = \
  libnux-core-@NUX_API_VERSION@.la

libnux_core_@NUX_API_VERSION@_la_CPPFLAGS= \
  $(GCC_FLAGS) \
  $(NUX_CORE_CFLAGS) \
  $(MAINTAINER_CFLAGS) \
  $(COVERAGE_CFLAGS)

libnux_core_@NUX_API_VERSION@_la_LIBADD = \
  $(NUX_CORE_LIBS) -lpthread

libnux_core_@NUX_API_VERSION@_la_LDFLAGS = \
  $(NUX_LT_LDFLAGS) \
  $(COVERAGE_LDFLAGS)

source_cpp = \
  Animation.cpp \
  AnimationController.cpp \
  AsyncFileWriter.cpp \
  EasingCurve.cpp \
  TextString.cpp \
  TimeFunctions.cpp \
  Template.cpp \
  FilePath.cpp \
  FileManager/NSerializer.cpp \
  FileManager/NFileManagerGeneric.cpp \
  FileManager/NFileManagerGNU.cpp \
  SystemGNU.cpp \
  OutputDevice.cpp \
  Exception.cpp \
  Memory.cpp \
  Character/NUTF.cpp \
  Character/NUni.cpp \
  Character/NUnicode.cpp \
  Color.cpp \
  ColorPrivate.cpp \
  Colors.cpp \
  FileName.cpp \
  ThreadGNU.cpp \
  Parsing.cpp \
  Logger.cpp \
  LoggingWriter.cpp \
  NUniqueIndex.cpp \
  GlobalInitializer.cpp \
  ObjectType.cpp \
  Platform.cpp \
  Rect.cpp \
  FileIO.cpp \
  Process.cpp \
  NumberConversion.cpp \
  NuxCore.cpp \
  Object.cpp \
  Math/Algo.cpp \
  Math/Constants.cpp \
  Math/MathFunctions.cpp \
  Math/Line2D.cpp \
  Math/Bezier.cpp \
  Math/Spline.cpp \
  Math/Quaternion.cpp \
  Math/Matrix2.cpp \
  Math/Trigonometry.cpp \
  Math/Tweening.cpp \
  Math/Complex.cpp \
  Math/Point3D.cpp \
  Math/Matrix4.cpp \
  Math/Matrix3.cpp \
  Math/Line3D.cpp \
  RollingFileAppender.cpp \
  InitiallyUnownedObject.cpp

source_file_manager_h = \
  FileManager/NFileManagerGNU.h \
  FileManager/NFileManagerGeneric.h \
  FileManager/NSerializer.h

source_character_h = \
  Character/NUTF.h \
  Character/NUni.h \
  Character/NUnicode.h \
  Character/NTChar.h

source_math_h = \
  Math/Algo.h \
  Math/Bezier.h \
  Math/Constants.h \
  Math/Point3D.h \
  Math/Vector4.h \
  Math/Matrix2.h \
  Math/Spline.h \
  Math/Vector3.h \
  Math/MathFunctions.h \
  Math/Quaternion.h \
  Math/Trigonometry.h \
  Math/Line3D.h \
  Math/MathInc.h \
  Math/Complex.h \
  Math/Matrix3.h \
  Math/Vector2.h \
  Math/Line2D.h \
  Math/Point2D.h \
  Math/Point2D-inl.h \
  Math/Tweening.h \
  Math/Matrix4.h \
  Math/MathUtility.h

source_h = \
  Animation.h \
  Animation-inl.h \
  AnimationController.h \
  AsyncFileWriter.h \
  EasingCurve.h \
  Point.h \
  FilePath.h \
  System.h \
  Parsing.h \
  StringConversion.h \
  GlobalInitializer.h \
  Rect.h \
  Platform.h \
  Inalogic.h \
  Color.h \
  Colors.h \
  Logger.h \
  LoggerPrivate.h \
  LoggingWriter.h \
  RollingFileAppender.h \
  Memory.h \
  Error.h \
  SystemGNU.h \
  NuxCore.h \
  FileName.h \
  OutputDevice.h \
  Process.h \
  ObjectType.h \
  ObjectPtr.h \
  NumberConversion.h \
  Object.h \
  Size.h \
  Size-inl.h \
  Exception.h \
  TimeFunctions.h \
  Macros.h \
  ThreadGNU.h \
  TextString.h \
  Template.h \
  NUniqueIndex.h \
  FileIO.h \
  InitiallyUnownedObject.h \
  Property.h \
  Property-inl.h \
  PropertyAnimation.h \
  PropertyOperators.h \
  PropertyTraits.h \
  Win32Dialogs/NWin32Clipboard.h

libnux_core_@NUX_API_VERSION@_la_SOURCES = \
  $(source_cpp) \
  $(source_h) \
  $(source_character_h) \
  $(source_file_manager_h) \
  $(source_math_h) \
  $(source_memory_h)

nux_coredir = $(includedir)/Nux-@NUX_API_VERSION@/NuxCore
nux_core_HEADERS = $(source_h)

nux_chardir = $(nux_coredir)/Character
nux_char_HEADERS = $(source_character_h)

nux_file_managerdir = $(nux_coredir)/FileManager
nux_file_manager_HEADERS = $(source_file_manager_h)

nux_mathdir = $(nux_coredir)/Math
nux_math_HEADERS = $(source_math_h)

nux_memorydir = $(nux_coredir)/Memory
nux_memory_HEADERS = $(source_memory_h)

# pkg-config file
nux-core-@NUX_API_VERSION@.pc: nux-core.pc
	$(AM_V_GEN) cp -f nux-core.pc nux-core-@NUX_API_VERSION@.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = nux-core-@NUX_API_VERSION@.pc

unused_src = \
 Character/NAscii.cpp \
 Character/NAscii.h \
 Character/NUnicodeGNU.cpp \
 Character/NUnicodeGNU.h \
 CppReadme.txt \
 FileManager/NFileManagerWindows.cpp \
 FileManager/NFileManagerWindows.h \
 Plugin/NPlugin.h \
 Plugin/NPluginInterface.h \
 Plugin/NPluginManager.cpp \
 Plugin/NPluginManager.h \
 Plugin/NPluging.cpp \
 SystemWindows.cpp \
 SystemWindows.h \
 ThreadWin.cpp \
 ThreadWin.h \
 Win32Dialogs/NWin32Clipboard.cpp \
 Win32Dialogs/NWin32CustomDialog.cpp \
 Win32Dialogs/NWin32CustomDialog.h \
 Win32Dialogs/NWin32MessageBox.cpp \
 Win32Dialogs/NWin32MessageBox.h

CLEANFILES += nux-core-@NUX_API_VERSION@.pc
DISTCLEANFILES += nux-core.pc
EXTRA_DIST += nux-core.pc.in ${unused_src}
