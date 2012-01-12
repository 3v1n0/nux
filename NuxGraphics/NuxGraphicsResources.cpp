/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#include "GLResource.h"
#include "FontTexture.h"
#include "NuxGraphicsResources.h"
#include "GLResourceManager.h"

namespace nux
{

  NUX_IMPLEMENT_GLOBAL_OBJECT(NuxGraphicsResources);

  void NuxGraphicsResources::Constructor()
  {

  }

  void NuxGraphicsResources::Destructor()
  {

  }

  void NuxGraphicsResources::InitializeResources()
  {
//         FilePath m_FilePath;
//         m_FilePath.AddSearchPath(""); // for case where fully qualified path is given
//         m_FilePath.AddSearchPath("./ini");
//         m_FilePath.AddSearchPath("../ini");
//         m_FilePath.AddSearchPath("../../ini");
//         m_FilePath.AddSearchPath("../../../ini");
//
//
//         NString file_search = "inalogic.ini";
//         NString FileName = m_FilePath.GetFile(file_search.GetTCharPtr());
//         if (FileName == "")
//         {
//             nuxCriticalMsg("[GraphicsDisplay::GraphicsDisplay] Can't find inalogic.ini file.");
// //             exit(-1);
//         }
//
//         if (FileName != "")
//         {
//             NString key_name = "UseBackupFBO";
//             NString section_name = "OpenGLSystem";
//
//             if (CIniFile::RecordExists(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr()) == false)
//             {
//                 nuxDebugMsg("[GraphicsDisplay::GraphicsDisplay] Key [%s] does not exit in .ini file.");
//             }
//             else
//             {
//                 bool m_BackupFrameBuffer;
//                 NString value = CIniFile::GetValue(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr());
//                 if ((value == "1") || (value == "true"))
//                 {
//                     m_BackupFrameBuffer = true;
//                 }
//                 else
//                 {
//                     m_BackupFrameBuffer = false;
//                 }
//             }
//
//             section_name = "SearchPath";
//             key_name = "FontPath";
//             if (CIniFile::RecordExists(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr()))
//             {
//                 NString FontPath = CIniFile::GetValue(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr());
//                 FontPath.ParseToArray(m_FontSearchPath, ";");
//             }
//             else
//             {
//                 nuxCriticalMsg("[GraphicsDisplay::GraphicsDisplay] Failed to read font search path from .ini file.");
// //                 return;
//
//             }
//
//             key_name = "UITexturePath";
//             if (CIniFile::RecordExists(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr()))
//             {
//                 NString UITexturePath = CIniFile::GetValue(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr());
//                 UITexturePath.ParseToArray(m_UITextureSearchPath, ";");
//             }
//             else
//             {
//                 nuxCriticalMsg("[GraphicsDisplay::GraphicsDisplay] Failed to read texture search path from .ini file.");
// //                 return;
//             }
//
//             key_name = "ShaderPath";
//             if (CIniFile::RecordExists(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr()))
//             {
//                 NString ShaderPath = CIniFile::GetValue(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr());
//                 ShaderPath.ParseToArray(m_ShaderSearchPath, ";");
//             }
//             else
//             {
//                 nuxCriticalMsg("[GraphicsDisplay::GraphicsDisplay] Failed to read shader search path from .ini file.");
// //                 return;
//             }
//         }

#ifdef NUX_OS_LINUX
    m_ResourcePathLocation.AddSearchPath(PKGDATADIR);
    m_ResourcePathLocation.AddSearchPath(PKGDATADIR"/Fonts");
    m_ResourcePathLocation.AddSearchPath(PKGDATADIR"/Meshes");
    m_ResourcePathLocation.AddSearchPath(PKGDATADIR"/UITextures");

#ifdef NUX_DEBUG
    /* For examples */
    m_ResourcePathLocation.AddSearchPath("../data");
    m_ResourcePathLocation.AddSearchPath("../data/Fonts");
    m_ResourcePathLocation.AddSearchPath("../data/Meshes");
    m_ResourcePathLocation.AddSearchPath("../data/UITextures");
#endif

#else
    m_ResourcePathLocation.AddSearchPath("");
    m_ResourcePathLocation.AddSearchPath("./");
    m_ResourcePathLocation.AddSearchPath("../");
    m_ResourcePathLocation.AddSearchPath("../../");
    m_ResourcePathLocation.AddSearchPath("./Data");
    m_ResourcePathLocation.AddSearchPath("../Data");
    m_ResourcePathLocation.AddSearchPath("../../Data");
    m_ResourcePathLocation.AddSearchPath("../../../Data");
    m_ResourcePathLocation.AddSearchPath("./Data/Fonts");
    m_ResourcePathLocation.AddSearchPath("../Data/Fonts");
    m_ResourcePathLocation.AddSearchPath("../../Data/Fonts");
    m_ResourcePathLocation.AddSearchPath("../../../Data/Fonts");
    m_ResourcePathLocation.AddSearchPath("./Data/UITextures");
    m_ResourcePathLocation.AddSearchPath("../Data/UITextures");
    m_ResourcePathLocation.AddSearchPath("../../Data/UITextures");
    m_ResourcePathLocation.AddSearchPath("../../../Data/UITextures");
#endif

    m_ResourcePathLocation.AddSearchPath(m_FontSearchPath);
    m_ResourcePathLocation.AddSearchPath(m_ShaderSearchPath);
    m_ResourcePathLocation.AddSearchPath(m_UITextureSearchPath);
  }

#ifdef NUX_OS_WINDOWS
  NString NuxGraphicsResources::FindResourceLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    NString path = m_ResourcePathLocation.GetFile(ResourceFileName);

    if (path == "" && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate resource file: %s.", ResourceFileName);
      return NString("");
    }

    return path;
  }

  NString NuxGraphicsResources::FindUITextureLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_UITextureSearchPath);
    NString path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate ui texture file: %s.", ResourceFileName);
      return NString("");
    }

    return path;
  }

  NString NuxGraphicsResources::FindShaderLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_ShaderSearchPath);
    NString path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate shader file: %s.", ResourceFileName);
      return NString("");
    }

    return path;
  }

  NString NuxGraphicsResources::FindFontLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_FontSearchPath);
    NString path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate font file file: %s.", ResourceFileName);
      return NString("");
    }

    return path;
  }
#elif defined(NUX_OS_LINUX)
  NString NuxGraphicsResources::FindResourceLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    NString path = m_ResourcePathLocation.GetFile(ResourceFileName);

    if (path == "" && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate resource file: %s.", ResourceFileName);
      return NString("");
    }

    return path;
  }

  NString NuxGraphicsResources::FindUITextureLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_UITextureSearchPath);
    NString path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate ui texture file: %s.", ResourceFileName);
      return NString("");
    }

    return path;
  }

  NString NuxGraphicsResources::FindShaderLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_ShaderSearchPath);
    NString path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate shader file: %s.", ResourceFileName);
      return NString("");
    }

    return path;
  }

  NString NuxGraphicsResources::FindFontLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_FontSearchPath);
    NString path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate font file file: %s.", ResourceFileName);
      return NString("");
    }

    return path;
  }
#endif

}
