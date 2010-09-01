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
  include_directories(/usr/include/cairo)
endif(UNIX)

include_directories(${InalogicBuild_SOURCE_DIR}
                    ${InalogicBuild_SOURCE_DIR}/NuxCore
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/Math
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/Character
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/FileManager
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/SmartPtr
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/TinyXML
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/Win32Dialogs
                    ${InalogicBuild_SOURCE_DIR}/NuxImage)

if(WIN32)
    set (SOURCES
    BitmapFormats.cpp
    BitmapFormats.h
    Bmp.cpp
    Bmp.h
    CairoGraphics.cpp
    CairoGraphics.h
    DDS.cpp
    DDS.h
    ImageSurface.cpp
    ImageSurface.h
    NAnimatedTexture.cpp
    NAnimatedTexture.h
    NITX.cpp
    NITX.h
    NPng.cpp
    NPng.h
    OpenEXR.cpp
    OpenEXR.h
    RGBE.cpp
    RGBE.h
    Tga.cpp
    Tga.h)
elseif(UNIX)
    set (SOURCES
    BitmapFormats.cpp
    BitmapFormats.h
    Bmp.cpp
    Bmp.h
    CairoGraphics.cpp
    CairoGraphics.h
    DDS.cpp
    DDS.h
    ImageSurface.cpp
    ImageSurface.h
    NAnimatedTexture.cpp
    NAnimatedTexture.h
    NITX.cpp
    NITX.h
    NPng.cpp
    NPng.h
    OpenEXR.cpp
    OpenEXR.h
    RGBE.cpp
    RGBE.h
    Tga.cpp
    Tga.h)
endif()


if(WIN32)
    add_library (NuxImage ${SOURCES}) 
elseif(UNIX)
    add_library (NuxImage ${SOURCES}) 
endif()


