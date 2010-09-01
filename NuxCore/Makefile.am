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

source_h = \
  $(srcdir)/NSystemTypes.h \
  $(srcdir)/Point.h \
  $(srcdir)/TinyXML/tinystr.h \
  $(srcdir)/TinyXML/tinyxml.h \
  $(srcdir)/NCPU.h \
  $(srcdir)/Memory/NMemoryHook.h \
  $(srcdir)/Memory/NDefaultMemoryAllocator.h \
  $(srcdir)/Memory/NMemoryDebugHook.h \
  $(srcdir)/Memory/NMemoryAllocatorInterface.h \
  $(srcdir)/Memory/NMemoryAllocator.h \
  $(srcdir)/FileManager/NFileManagerGNU.h \
  $(srcdir)/FileManager/NFileManagerGeneric.h \
  $(srcdir)/FileManager/NSerializer.h \
  $(srcdir)/BitmapFormats.h \
  $(srcdir)/FilePath.h \
  $(srcdir)/NSystem.h \
  $(srcdir)/NParsing.h \
  $(srcdir)/NStringConversion.h \
  $(srcdir)/NGlobalInitializer.h \
  $(srcdir)/NPrintf.h \
  $(srcdir)/Rect.h \
  $(srcdir)/NPlatform.h \
  $(srcdir)/SmartPtr/NRefCount.h \
  $(srcdir)/SmartPtr/NSmartPtr.h \
  $(srcdir)/DataStruct/NList.h \
  $(srcdir)/Inalogic.h \
  $(srcdir)/Color.h \
  $(srcdir)/NArray.h \
  $(srcdir)/NStreamBuffer.h \
  $(srcdir)/Character/NUTF.h \
  $(srcdir)/Character/NUni.h \
  $(srcdir)/Character/NUnicode.h \
  $(srcdir)/Character/NTChar.h \
  $(srcdir)/NMemory.h \
  $(srcdir)/Error.h \
  $(srcdir)/NNamespace.h \
  $(srcdir)/NSystemGNU.h \
  $(srcdir)/NKernel.h \
  $(srcdir)/NFileName.h \
  $(srcdir)/NOutputDevice.h \
  $(srcdir)/NProcess.h \
  $(srcdir)/NObjectType.h \
  $(srcdir)/ColorFunctions.h \
  $(srcdir)/Size.h \
  $(srcdir)/Exception.h \
  $(srcdir)/NSystemPS3.h \
  $(srcdir)/Math/Constants.h \
  $(srcdir)/Math/Bezier.h \
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
  $(srcdir)/Math/MathUtility.h \
  $(srcdir)/NTime.h \
  $(srcdir)/NCRC32.h \
  $(srcdir)/NMacros.h \
  $(srcdir)/NThreadGNU.h \
  $(srcdir)/NString.h \
  $(srcdir)/NTemplate.h \
  $(srcdir)/NUniqueIndex.h \
  $(srcdir)/NFile.h

source_cpp = \
  $(srcdir)/NString.cpp \
  $(srcdir)/NTime.cpp \
  $(srcdir)/NTemplate.cpp \
  $(srcdir)/FilePath.cpp \
  $(srcdir)/TinyXML/tinyxmlparser.cpp \
  $(srcdir)/TinyXML/tinyxml.cpp \
  $(srcdir)/TinyXML/tinystr.cpp \
  $(srcdir)/TinyXML/tinyxmlerror.cpp \
  $(srcdir)/Memory/NMemoryAllocator.cpp \
  $(srcdir)/Memory/NMemoryHook.cpp \
  $(srcdir)/Memory/NDefaultMemoryAllocator.cpp \
  $(srcdir)/Memory/NMemoryDebugHook.cpp \
  $(srcdir)/Memory/NMemoryAllocatorInterface.cpp \
  $(srcdir)/FileManager/NSerializer.cpp \
  $(srcdir)/FileManager/NFileManagerGeneric.cpp \
  $(srcdir)/FileManager/NFileManagerGNU.cpp \
  $(srcdir)/NKernel.cpp \
  $(srcdir)/NSystemGNU.cpp \
  $(srcdir)/SmartPtr/NRefCount.cpp \
  $(srcdir)/SmartPtr/NSmartPtr.cpp \
  $(srcdir)/NOutputDevice.cpp \
  $(srcdir)/Exception.cpp \
  $(srcdir)/NMemory.cpp \
  $(srcdir)/NStreamBuffer.cpp \
  $(srcdir)/Character/NUTF.cpp \
  $(srcdir)/Character/NUni.cpp \
  $(srcdir)/Character/NUnicode.cpp \
  $(srcdir)/Color.cpp \
  $(srcdir)/NFileName.cpp \
  $(srcdir)/NThreadGNU.cpp \
  $(srcdir)/NParsing.cpp \
  $(srcdir)/NCPU.cpp \
  $(srcdir)/Error.cpp \
  $(srcdir)/ColorFunctions.cpp \
  $(srcdir)/NUniqueIndex.cpp \
  $(srcdir)/Point.cpp \
  $(srcdir)/NGlobalInitializer.cpp \
  $(srcdir)/NObjectType.cpp \
  $(srcdir)/NArray.cpp \
  $(srcdir)/NPlatform.cpp \
  $(srcdir)/Rect.cpp \
  $(srcdir)/NPrintf.cpp \
  $(srcdir)/NFile.cpp \
  $(srcdir)/NProcess.cpp \
  $(srcdir)/Math/Point2D.cpp \
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
  $(srcdir)/NCRC32.cpp \
  $(srcdir)/BitmapFormats.cpp \
  $(srcdir)/Size.cpp

libnux_core_@NUX_API_VERSION@_la_SOURCES = \
  $(source_cpp) \
  $(source_h)

nux_coredir = $(includedir)/Nux-@NUX_API_VERSION@/NuxCore
nux_core_HEADERS = \
  $(source_h)
