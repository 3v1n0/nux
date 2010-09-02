/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the applicable version of the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of both the GNU Lesser General Public 
 * License version 3 along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef NWORLDARCHIVEMANAGER_H
#define NWORLDARCHIVEMANAGER_H

#include "NuxCore/FileManager/NSerializer.h"

NAMESPACE_BEGIN
class NMeshObject;
class N3DWorld;

typedef enum
{
    OBJECTTYPE_UNKNOWN  = 0,
    OBJECTTYPE_MESH,
    OBJECTTYPE_LIGHT,
    FORCE_OBJECTTYPE = 0x7fffffff
} ObjectType;

#define WORLDARCHIVE_TAG            INL_MAKEFOURCHARTAG('i','w','l','d')       // .iwl
#define WORLDARCHIVE_VERSION        INL_MAKEFOURCHARTAG('0','0','0','1')   
#define WORLDARCHIVE_END_TAG        INL_MAKEFOURCHARTAG(0,0,0,0)


#define STATICMESHARCHIVE_TAG       INL_MAKEFOURCHARTAG('i','m','s','h')       // .ims
#define STATICMESHARCHIVE_VERSION   INL_MAKEFOURCHARTAG('0','0','0','1')   
#define STATICMESHARCHIVE_END_TAG   INL_MAKEFOURCHARTAG(0,0,0,0)


typedef struct  
{
    unsigned int ArchiveTag;
    unsigned int ArchiveFormatVersion;
// Creation date
    unsigned int Year;
    unsigned int Month;
    unsigned int Day;
    unsigned int Hour;
    unsigned int Min;
    unsigned int Sec;
    unsigned int MSec;
} WorldAchiveHeader;

const unsigned int WORLDACHIVEHEADER_SIZE     = 36;

class NWorldArchiveManager
{
public:
    NWorldArchiveManager();
    ~NWorldArchiveManager();

    bool SaveWorldArchiveFile(const TCHAR* WorldArchiveFilename, N3DWorld* World);
    N3DWorld* LoadWorldArchiveFile(const TCHAR* WorldArchiveFilename);

    bool SaveStaticMeshFile(const TCHAR* MeshFilename, NMeshObject* Mesh, unsigned int Flags = 0);
    NMeshObject* LoadStaticMeshFile(const TCHAR* MeshFilename);


    bool m_isLoaded;
    nux::NSerializer* m_FileStream;
    unsigned int m_NumberOfObjects;
    NString m_CurrentWorldArchive;
};

NAMESPACE_END
#endif // NWORLDARCHIVEMANAGER_H
