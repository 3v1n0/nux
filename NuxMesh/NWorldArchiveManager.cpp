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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "NuxCore/NuxCore.h"

#include "NuxCore/FilePath.h"
#include "NWorldArchiveManager.h"
#include "NMeshArchiver_ver_0_0_1.h"
#include "NMeshObject.h"
#include "N3DWorld.h"

namespace nux
{

  NWorldArchiveManager::NWorldArchiveManager()
  {
  }

  NWorldArchiveManager::~NWorldArchiveManager()
  {
  }

  bool NWorldArchiveManager::SaveWorldArchiveFile (const TCHAR *WorldArchiveFilename, N3DWorld *World)
  {
    if (World == 0)
      return false;

#define CHECK_WORLD_WRITE_ERROR /*if(m_FileStream.bad()) {m_FileStream->close(); nuxAssertMsg(0, TEXT("[CreateTextureArchiveFile] Write Error.")); return false;}*/

    m_FileStream = GFileManager.CreateFileWriter (TCHAR_TO_ANSI (WorldArchiveFilename), nux::NSerializer::Read | nux::NSerializer::Write);

    if (!m_FileStream)
    {
      nuxAssertMsg (0, TEXT ("[SaveWorldArchiveFile] Can't open file: %s"), WorldArchiveFilename);
      m_isLoaded = false;
      return false;
    }

    m_FileStream->Seek (0, nux::NSerializer::SeekStart);
    unsigned int ArchiveTag = WORLDARCHIVE_TAG;
    unsigned int ArchiveFormatVersion = WORLDARCHIVE_VERSION;
    m_NumberOfObjects = 0;
    nux::NTimeStamp TimeStamp;
    TimeStamp.GetTime();

    m_FileStream->Serialize ( (char *) &ArchiveTag,            sizeof (ArchiveTag) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &ArchiveFormatVersion,  sizeof (ArchiveFormatVersion) );
    CHECK_WORLD_WRITE_ERROR;

    // Date Created
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );
    CHECK_WORLD_WRITE_ERROR;

    // Date Modified
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );
    CHECK_WORLD_WRITE_ERROR;

    // Number of Textures in the archives
    m_FileStream->Serialize ( (char *) &m_NumberOfObjects,        sizeof (m_NumberOfObjects) );
    CHECK_WORLD_WRITE_ERROR;

    int NumMesh = World->NumMesh();

    for (int i = 0; i < NumMesh; i++)
    {
      ObjectType objecttype = OBJECTTYPE_MESH;
      m_FileStream->Serialize ( (char *) &objecttype,      sizeof (objecttype) );
      SaveMeshObject_ver_0_0_1 (m_FileStream, World->GetMesh (i) );
    }

    unsigned int ArchiveEndTag = WORLDARCHIVE_END_TAG;
    m_FileStream->Serialize ( (char *) &ArchiveEndTag,      sizeof (ArchiveEndTag) );
    m_FileStream->Close();

#undef CHECK_WORLD_WRITE_ERROR

    return true;
  }

  N3DWorld *NWorldArchiveManager::LoadWorldArchiveFile (const TCHAR *WorldArchiveFilename)
  {
#define CHECK_WORLD_READ_ERROR /*if(m_FileStream.bad()) {m_FileStream->close(); nuxAssertMsg(0, TEXT("[CreateTextureArchiveFile] Write Error.")); return false;}*/

    m_FileStream = GFileManager.CreateFileReader (TCHAR_TO_ANSI (WorldArchiveFilename), nux::NSerializer::Read | nux::NSerializer::Write);

    if (!m_FileStream)
    {
      nuxAssertMsg (0, TEXT ("[LoadWorldArchiveFile] Can't open file: %s"), WorldArchiveFilename);
      return 0;
    }

    m_FileStream->Seek (0, nux::NSerializer::SeekStart);
    unsigned int ArchiveTag = 0;
    unsigned int ArchiveFormatVersion = 0;
    m_NumberOfObjects = 0;
    nux::NTimeStamp TimeStamp;

    m_FileStream->Serialize ( (char *) &ArchiveTag,            sizeof (ArchiveTag) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &ArchiveFormatVersion,  sizeof (ArchiveFormatVersion) );
    CHECK_WORLD_READ_ERROR;

    if (ArchiveTag != WORLDARCHIVE_TAG)
    {
      m_FileStream->Close();
      return 0;
    }

    // Date Created
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );
    CHECK_WORLD_READ_ERROR;

    // Date Modified
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    CHECK_WORLD_READ_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );
    CHECK_WORLD_READ_ERROR;

    // Number of Textures in the archives
    m_FileStream->Serialize ( (char *) &m_NumberOfObjects,        sizeof (m_NumberOfObjects) );
    CHECK_WORLD_READ_ERROR;


    N3DWorld *World = new N3DWorld();

    ObjectType objecttype;
    int tag = 0;
    m_FileStream->Serialize ( (char *) &tag,      sizeof (tag) );

    while (tag != 0)
    {
      objecttype = ObjectType (tag);
      t_s64 filepos = m_FileStream->Tell();

      if (objecttype == OBJECTTYPE_MESH)
      {
        NMeshObject *mesh = LoadMeshObject_ver_0_0_1 (m_FileStream, filepos);

        if (mesh)
        {
          mesh->CreateGLObjects();
          World->AddMeshObject (mesh);
        }
      }

      m_FileStream->Serialize ( (char *) &tag,      sizeof (tag) );
    }

    m_FileStream->Close();

    return World;
  }


  bool NWorldArchiveManager::SaveStaticMeshFile (const TCHAR *MeshFilename, NMeshObject *Mesh, unsigned int Flags)
  {
    if (Mesh == 0)
      return false;

#define CHECK_WORLD_WRITE_ERROR /*if(m_FileStream.bad()) {m_FileStream->close(); nuxAssertMsg(0, TEXT("[CreateTextureArchiveFile] Write Error.")); return false;}*/

    m_FileStream = GFileManager.CreateFileWriter (TCHAR_TO_ANSI (MeshFilename), Flags | nux::NSerializer::Read | nux::NSerializer::Write);

    if (!m_FileStream)
    {
      nuxAssertMsg (0, TEXT ("[SaveStaticMeshFile] Can't open file: %s"), MeshFilename);
      m_isLoaded = false;
      return false;
    }

    m_FileStream->Seek (0, nux::NSerializer::SeekStart);
    unsigned int ArchiveTag = STATICMESHARCHIVE_TAG;
    unsigned int ArchiveFormatVersion = STATICMESHARCHIVE_VERSION;
    nux::NTimeStamp TimeStamp;
    TimeStamp.GetTime();

    m_FileStream->Serialize ( (char *) &ArchiveTag,            sizeof (ArchiveTag) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &ArchiveFormatVersion,  sizeof (ArchiveFormatVersion) );
    CHECK_WORLD_WRITE_ERROR;

    // Date Created
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );
    CHECK_WORLD_WRITE_ERROR;

    // Date Modified
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    CHECK_WORLD_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );
    CHECK_WORLD_WRITE_ERROR;


    ObjectType objecttype = OBJECTTYPE_MESH;
    m_FileStream->Serialize ( (char *) &objecttype,      sizeof (objecttype) );
    SaveMeshObject_ver_0_0_1 (m_FileStream, Mesh);

    unsigned int ArchiveEndTag = STATICMESHARCHIVE_END_TAG;
    m_FileStream->Serialize ( (char *) &ArchiveEndTag,      sizeof (ArchiveEndTag) );
    m_FileStream->Close();

#undef CHECK_WORLD_WRITE_ERROR

    return true;
  }

  NMeshObject *NWorldArchiveManager::LoadStaticMeshFile (const TCHAR *MeshFilename)
  {
    m_FileStream = GFileManager.CreateFileReader (TCHAR_TO_ANSI (MeshFilename), nux::NSerializer::Read | nux::NSerializer::Write);

    if (!m_FileStream)
    {
      nuxAssertMsg (0, TEXT ("[LoadStaticMeshFile] Can't open file: %s"), MeshFilename);
      return 0;
    }

    m_FileStream->Seek (0, nux::NSerializer::SeekStart);
    unsigned int ArchiveTag = 0;
    unsigned int ArchiveFormatVersion = 0;
    m_NumberOfObjects = 0;
    nux::NTimeStamp TimeStamp;

    m_FileStream->Serialize ( (char *) &ArchiveTag,            sizeof (ArchiveTag) );
    m_FileStream->Serialize ( (char *) &ArchiveFormatVersion,  sizeof (ArchiveFormatVersion) );

    if (ArchiveTag != STATICMESHARCHIVE_TAG)
    {
      m_FileStream->Close();
      return 0;
    }

    // Date Created
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );

    // Date Modified
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );

    NMeshObject *Mesh = 0;
    ObjectType objecttype;
    m_FileStream->Serialize ( (char *) &objecttype,      sizeof (objecttype) );

    bool MeshExtracted = false;

    if (objecttype == OBJECTTYPE_MESH)
    {
      t_s64 filepos = m_FileStream->Tell();
      Mesh = LoadMeshObject_ver_0_0_1 (m_FileStream, filepos);

      if (Mesh)
      {
        Mesh->CreateGLObjects();
      }

      MeshExtracted = true;
    }
    else
    {
      nuxAssertMsg (0, TEXT ("[LoadStaticMeshFile] Can't extract static mesh from mesh archive: %s"), MeshFilename);
    }

    m_FileStream->Close();

    if (MeshExtracted == false)
    {
      return 0;
    }

    return Mesh;
  }



}

