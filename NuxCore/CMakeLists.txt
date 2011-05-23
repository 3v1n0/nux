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
endif(UNIX)

include_directories(${InalogicBuild_SOURCE_DIR}
                    ${InalogicBuild_SOURCE_DIR}/NuxCore
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/Math
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/Character
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/FileManager
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/SmartPtr
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/TinyXML
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/Win32Dialogs)

if(WIN32)
    set (SOURCES
    Character/NAscii.cpp
    Character/NAscii.h
    Character/NTChar.h
    Character/NUnicode.cpp
    Character/NUnicode.h
    Character/NUTF.cpp
    Character/NUTF.h
    Character/NUni.cpp
    Character/NUni.h

    FileManager/NFileManagerGeneric.cpp
    FileManager/NFileManagerGeneric.h
    FileManager/NFileManagerStandardAnsi.cpp
    FileManager/NFileManagerStandardAnsi.h
    FileManager/NFileManagerWindows.cpp
    FileManager/NFileManagerWindows.h
    FileManager/NSerializer.cpp
    FileManager/NSerializer.h
    
    Memory/NDefaultMemoryAllocator.cpp
    Memory/NDefaultMemoryAllocator.h
    Memory/NMemoryAllocator.cpp
    Memory/NMemoryAllocator.h
    Memory/NMemoryAllocatorInterface.cpp
    Memory/NMemoryAllocatorInterface.h
    Memory/NMemoryDebugHook.cpp
    Memory/NMemoryDebugHook.h
    Memory/NMemoryHook.cpp
    Memory/NMemoryHook.h

    Math/Bezier.cpp
    Math/Bezier.h
    Math/Complex.cpp
    Math/Complex.h
    Math/Constants.h
    Math/Line2D.cpp
    Math/Line2D.h
    Math/Line3D.cpp
    Math/Line3D.h
    Math/MathFunctions.cpp
    Math/MathFunctions.h
    Math/MathInc.h
    Math/MathUtility.h
    Math/Matrix2.cpp
    Math/Matrix2.h
    Math/Matrix3.cpp
    Math/Matrix3.h
    Math/Matrix4.cpp
    Math/Matrix4.h
    Math/Point2D.cpp
    Math/Point2D.h
    Math/Point3D.cpp
    Math/Point3D.h
    Math/Quaternion.cpp
    Math/Quaternion.h
    Math/Spline.cpp
    Math/Spline.h
    Math/Trigonometry.cpp
    Math/Trigonometry.h
    Math/Vector2.cpp
    Math/Vector2.h
    Math/Vector3.cpp
    Math/Vector3.h
    Math/Vector4.cpp
    Math/Vector4.h
    
    SmartPtr/NRefCount.cpp
    SmartPtr/NRefCount.h
    SmartPtr/NSmartPtr.cpp
    SmartPtr/NSmartPtr.h
    
    Win32Dialogs/NWin32Clipboard.cpp
    Win32Dialogs/NWin32Clipboard.h
    Win32Dialogs/NWin32CustomDialog.cpp
    Win32Dialogs/NWin32CustomDialog.h
    Win32Dialogs/NWin32MessageBox.cpp
    Win32Dialogs/NWin32MessageBox.h
    
    TinyXML/tinystr.cpp
    TinyXML/tinystr.h
    TinyXML/tinyxml.cpp
    TinyXML/tinyxml.h
    TinyXML/tinyxmlerror.cpp
    TinyXML/tinyxmlparser.cpp
 
    BitmapFormats.cpp
    BitmapFormats.h
    Color.cpp
    Color.h
    CppReadme.txt
    Error.cpp
    Error.h
    Exception.cpp
    Exception.h
    FilePath.cpp
    FilePath.h
    Inalogic.h
    NArray.cpp
    NArray.h
    NCPU.cpp
    NCPU.h
    NCRC32.cpp
    NCRC32.h
    NFile.cpp
    NFile.h
    NFileName.cpp
    NFileName.h
    NGlobalInitializer.cpp
    NGlobalInitializer.h
    NKernel.cpp
    NKernel.h
    NMacros.h
    NMemory.cpp
    NMemory.h
    NNamespace.h
    NObjectType.cpp
    NObjectType.h
    NOutputDevice.cpp
    NOutputDevice.h
    NParsing.cpp
    NParsing.h
    NPlatform.cpp
    NPlatform.h
    NPrintf.cpp
    NPrintf.h
    NProcess.cpp
    NProcess.h
    NStreamBuffer.cpp
    NStreamBuffer.h
    NString.cpp
    NString.h
    NStringConversion.h
    NSystem.h
    NSystemTypes.h
    NSystemWindows.cpp
    NSystemWindows.h
    NTemplate.cpp
    NTemplate.h
    NThread.cpp
    NThread.h
    NTime.cpp
    NTime.h
    NUniqueIndex.cpp
    NUniqueIndex.h
    Point.cpp
    Point.h
    Rect.cpp
    Rect.h
    Size.cpp
    Size.h
    )
elseif(UNIX)
    set (SOURCES
    Character/NAscii.cpp
    Character/NAscii.h
    Character/NTChar.h
    Character/NUnicode.cpp
    Character/NUnicode.h
    Character/NUTF.cpp
    Character/NUTF.h
    Character/NUni.cpp
    Character/NUni.h
        
    FileManager/NFileManagerGeneric.cpp
    FileManager/NFileManagerGeneric.h
#    FileManager/NFileManagerStandardAnsi.cpp
#    FileManager/NFileManagerStandardAnsi.h
    FileManager/NFileManagerGNU.cpp
    FileManager/NFileManagerGNU.h
    FileManager/NSerializer.cpp
    FileManager/NSerializer.h
    
    Memory/NDefaultMemoryAllocator.cpp
    Memory/NDefaultMemoryAllocator.h
    Memory/NMemoryAllocator.cpp
    Memory/NMemoryAllocator.h
    Memory/NMemoryAllocatorInterface.cpp
    Memory/NMemoryAllocatorInterface.h
    Memory/NMemoryDebugHook.cpp
    Memory/NMemoryDebugHook.h
    Memory/NMemoryHook.cpp
    Memory/NMemoryHook.h
    
    Math/Bezier.cpp
    Math/Bezier.h
    Math/Complex.cpp
    Math/Complex.h
    Math/Constants.h
    Math/Line2D.cpp
    Math/Line2D.h
    Math/Line3D.cpp
    Math/Line3D.h
    Math/MathFunctions.cpp
    Math/MathFunctions.h
    Math/MathInc.h
    Math/MathUtility.h
    Math/Matrix2.cpp
    Math/Matrix2.h
    Math/Matrix3.cpp
    Math/Matrix3.h
    Math/Matrix4.cpp
    Math/Matrix4.h
    Math/Point2D.cpp
    Math/Point2D.h
    Math/Point3D.cpp
    Math/Point3D.h
    Math/Quaternion.cpp
    Math/Quaternion.h
    Math/Spline.cpp
    Math/Spline.h
    Math/Trigonometry.cpp
    Math/Trigonometry.h
    Math/Vector2.cpp
    Math/Vector2.h
    Math/Vector3.cpp
    Math/Vector3.h
    Math/Vector4.cpp
    Math/Vector4.h
    
    SmartPtr/NRefCount.cpp
    SmartPtr/NRefCount.h
    SmartPtr/NSmartPtr.cpp
    SmartPtr/NSmartPtr.h
    
    TinyXML/tinystr.cpp
    TinyXML/tinystr.h
    TinyXML/tinyxml.cpp
    TinyXML/tinyxml.h
    TinyXML/tinyxmlerror.cpp
    TinyXML/tinyxmlparser.cpp
    
    BitmapFormats.cpp
    BitmapFormats.h
    Color.cpp
    Color.h
    CppReadme.txt
    Error.cpp
    Error.h
    Exception.cpp
    Exception.h
    FilePath.cpp
    FilePath.h
    Inalogic.h
    NArray.cpp
    NArray.h
    NCPU.cpp
    NCPU.h
    NCRC32.cpp
    NCRC32.h
    NFile.cpp
    NFile.h
    NFileName.cpp
    NFileName.h
    NGlobalInitializer.cpp
    NGlobalInitializer.h
    NKernel.cpp
    NKernel.h
    NMacros.h
    NMemory.cpp
    NMemory.h
    NNamespace.h
    NObjectType.cpp
    NObjectType.h
    NOutputDevice.cpp
    NOutputDevice.h
    NParsing.cpp
    NParsing.h
    NPlatform.cpp
    NPlatform.h
    NPrintf.cpp
    NPrintf.h
    NProcess.cpp
    NProcess.h
    NStreamBuffer.cpp
    NStreamBuffer.h
    NString.cpp
    NString.h
    NStringConversion.h
    NSystem.h
    NSystemTypes.h
    NSystemGNU.cpp
    NSystemGNU.h
    NTemplate.cpp
    NTemplate.h
    NThreadGNU.cpp
    NThreadGNU.h
    NTime.cpp
    NTime.h
    NUniqueIndex.cpp
    NUniqueIndex.h
    Point.cpp
    Point.h
    Rect.cpp
    Rect.h
    Size.cpp
    Size.h
    )
endif()


if(WIN32)
    add_library (NuxCore ${SOURCES}) 
elseif(UNIX)
    add_library (NuxCore ${SOURCES}) 
endif()


