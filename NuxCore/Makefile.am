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
  $(MAINTAINER_CFLAGS)

libnux_core_@NUX_API_VERSION@_la_LIBADD = \
  $(NUX_CORE_LIBS)

libnux_core_@NUX_API_VERSION@_la_LDFLAGS = \
  $(NUX_LT_LDFLAGS)

source_cpp = \
  $(srcdir)/TextString.cpp \
  $(srcdir)/TimeFunctions.cpp \
  $(srcdir)/Template.cpp \
  $(srcdir)/FilePath.cpp \
  $(srcdir)/TinyXML/tinyxmlparser.cpp \
  $(srcdir)/TinyXML/tinyxml.cpp \
  $(srcdir)/TinyXML/tinystr.cpp \
  $(srcdir)/TinyXML/tinyxmlerror.cpp \
  $(srcdir)/FileManager/NSerializer.cpp \
  $(srcdir)/FileManager/NFileManagerGeneric.cpp \
  $(srcdir)/FileManager/NFileManagerGNU.cpp \
  $(srcdir)/SystemGNU.cpp \
  $(srcdir)/OutputDevice.cpp \
  $(srcdir)/Exception.cpp \
  $(srcdir)/Memory.cpp \
  $(srcdir)/StreamBuffer.cpp \
  $(srcdir)/Character/NUTF.cpp \
  $(srcdir)/Character/NUni.cpp \
  $(srcdir)/Character/NUnicode.cpp \
  $(srcdir)/Color.cpp \
  $(srcdir)/ColorFunctions.cpp \
  $(srcdir)/Colors.cpp \
  $(srcdir)/FileName.cpp \
  $(srcdir)/ThreadGNU.cpp \
  $(srcdir)/Parsing.cpp \
  $(srcdir)/CPU.cpp \
  $(srcdir)/Error.cpp \
  $(srcdir)/NUniqueIndex.cpp \
  $(srcdir)/GlobalInitializer.cpp \
  $(srcdir)/ObjectType.cpp \
  $(srcdir)/Platform.cpp \
  $(srcdir)/Rect.cpp \
  $(srcdir)/FileIO.cpp \
  $(srcdir)/Process.cpp \
  $(srcdir)/NumberConversion.cpp \
  $(srcdir)/NuxCore.cpp \
  $(srcdir)/Object.cpp \
  $(srcdir)/Math/Algo.cpp \
  $(srcdir)/Math/Constants.cpp \
  $(srcdir)/Math/MathFunctions.cpp \
  $(srcdir)/Math/Line2D.cpp \
  $(srcdir)/Math/Bezier.cpp \
  $(srcdir)/Math/Spline.cpp \
  $(srcdir)/Math/Quaternion.cpp \
  $(srcdir)/Math/Matrix2.cpp \
  $(srcdir)/Math/Trigonometry.cpp \
  $(srcdir)/Math/Tweening.cpp \
  $(srcdir)/Math/Vector4.cpp \
  $(srcdir)/Math/Complex.cpp \
  $(srcdir)/Math/Point3D.cpp \
  $(srcdir)/Math/Matrix4.cpp \
  $(srcdir)/Math/Matrix3.cpp \
  $(srcdir)/Math/Vector3.cpp \
  $(srcdir)/Math/Line3D.cpp \
  $(srcdir)/Math/Vector2.cpp \
  $(srcdir)/CRC32.cpp \
  $(srcdir)/InitiallyUnownedObject.cpp

source_file_manager_h = \
  $(srcdir)/FileManager/NFileManagerGNU.h \
  $(srcdir)/FileManager/NFileManagerGeneric.h \
  $(srcdir)/FileManager/NSerializer.h

source_character_h = \
  $(srcdir)/Character/NUTF.h \
  $(srcdir)/Character/NUni.h \
  $(srcdir)/Character/NUnicode.h \
  $(srcdir)/Character/NTChar.h

source_math_h = \
  $(srcdir)/Math/Algo.h \
  $(srcdir)/Math/Bezier.h \
  $(srcdir)/Math/Constants.h \
  $(srcdir)/Math/Point3D.h \
  $(srcdir)/Math/Vector4.h \
  $(srcdir)/Math/Matrix2.h \
  $(srcdir)/Math/Spline.h \
  $(srcdir)/Math/Vector3.h \
  $(srcdir)/Math/MathFunctions.h \
  $(srcdir)/Math/Quaternion.h \
  $(srcdir)/Math/Trigonometry.h \
  $(srcdir)/Math/Line3D.h \
  $(srcdir)/Math/MathInc.h \
  $(srcdir)/Math/Complex.h \
  $(srcdir)/Math/Matrix3.h \
  $(srcdir)/Math/Vector2.h \
  $(srcdir)/Math/Line2D.h \
  $(srcdir)/Math/Point2D.h \
  $(srcdir)/Math/Tweening.h \
  $(srcdir)/Math/Matrix4.h \
  $(srcdir)/Math/MathUtility.h

source_tiny_xml_h = \
  $(srcdir)/TinyXML/tinystr.h \
  $(srcdir)/TinyXML/tinyxml.h

source_smart_ptr_h = \
  $(srcdir)/SmartPtr/GenericSmartPointer.h

source_datastruct_h = \
  $(srcdir)/DataStruct/NList.h

source_h = \
  $(srcdir)/SystemTypes.h \
  $(srcdir)/Point.h \
  $(srcdir)/CPU.h \
  $(srcdir)/FilePath.h \
  $(srcdir)/System.h \
  $(srcdir)/Parsing.h \
  $(srcdir)/StringConversion.h \
  $(srcdir)/GlobalInitializer.h \
  $(srcdir)/Rect.h \
  $(srcdir)/Platform.h \
  $(srcdir)/Inalogic.h \
  $(srcdir)/Color.h \
  $(srcdir)/ColorFunctions.h \
  $(srcdir)/Colors.h \
  $(srcdir)/StreamBuffer.h \
  $(srcdir)/Memory.h \
  $(srcdir)/Error.h \
  $(srcdir)/Namespace.h \
  $(srcdir)/SystemGNU.h \
  $(srcdir)/NuxCore.h \
  $(srcdir)/FileName.h \
  $(srcdir)/OutputDevice.h \
  $(srcdir)/Process.h \
  $(srcdir)/ObjectType.h \
  $(srcdir)/ObjectPtr.h \
  $(srcdir)/NumberConversion.h \
  $(srcdir)/Object.h \
  $(srcdir)/Size.h \
  $(srcdir)/Exception.h \
  $(srcdir)/TimeFunctions.h \
  $(srcdir)/CRC32.h \
  $(srcdir)/Macros.h \
  $(srcdir)/ThreadGNU.h \
  $(srcdir)/TextString.h \
  $(srcdir)/Template.h \
  $(srcdir)/NUniqueIndex.h \
  $(srcdir)/FileIO.h \
  $(srcdir)/InitiallyUnownedObject.h \
  $(srcdir)/Win32Dialogs/NWin32Clipboard.h

libnux_core_@NUX_API_VERSION@_la_SOURCES = \
  $(source_cpp) \
  $(source_h) \
  $(source_character_h) \
  $(source_file_manager_h) \
  $(source_math_h) \
  $(source_memory_h) \
  $(source_tiny_xml_h) \
  $(source_smart_ptr_h) \
  $(source_datastruct_h)

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

nux_tiny_xmldir = $(nux_coredir)/TinyXML
nux_tiny_xml_HEADERS = $(source_tiny_xml_h)

nux_smart_ptrdir = $(nux_coredir)/SmartPtr
nux_smart_ptr_HEADERS = $(source_smart_ptr_h)

nux_datastructdir = $(nux_coredir)/DataStruct
nux_datastruct_HEADERS = $(source_datastruct_h)

# pkg-config file
nux-core-@NUX_API_VERSION@.pc: nux-core.pc
	$(AM_V_GEN) cp -f nux-core.pc nux-core-@NUX_API_VERSION@.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = nux-core-@NUX_API_VERSION@.pc

CLEANFILES += nux-core-@NUX_API_VERSION@.pc
DISTCLEANFILES += nux-core.pc
EXTRA_DIST += nux-core.pc.in
