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
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/Win32Dialogs                    
                    ${InalogicBuild_SOURCE_DIR}/NuxCore/TinyXML
                    ${InalogicBuild_SOURCE_DIR}/NuxImage
                    ${InalogicBuild_SOURCE_DIR}/NuxMesh)

if(WIN32)
    set (SOURCES
	GLRenderableMesh.cpp
	GLRenderableMesh.h
	N3DWorld.cpp
	N3DWorld.h
	NMeshArchiver_ver_0_0_1.cpp
	NMeshArchiver_ver_0_0_1.h
	NMeshObject.cpp
	NMeshObject.h
	NTextureArchiveManager.cpp
	NTextureArchiveManager.h
	NWorldArchiveManager.cpp
	NWorldArchiveManager.h
	NWorldObject.cpp
	NWorldObject.h
	SceneData.cpp
	SceneData.h
	TextureArchive_ver_0_0_1.cpp
	TextureArchive_ver_0_0_1.h)
elseif(UNIX)
    set (SOURCES
	GLRenderableMesh.cpp
	GLRenderableMesh.h
	N3DWorld.cpp
	N3DWorld.h
	NMeshArchiver_ver_0_0_1.cpp
	NMeshArchiver_ver_0_0_1.h
	NMeshObject.cpp
	NMeshObject.h
	NTextureArchiveManager.cpp
	NTextureArchiveManager.h
	NWorldArchiveManager.cpp
	NWorldArchiveManager.h
	NWorldObject.cpp
	NWorldObject.h
	SceneData.cpp
	SceneData.h
	TextureArchive_ver_0_0_1.cpp
	TextureArchive_ver_0_0_1.h)
endif()


if(WIN32)
    add_library (NuxMesh ${SOURCES}) 
elseif(UNIX)
    add_library (NuxMesh ${SOURCES}) 
endif()


