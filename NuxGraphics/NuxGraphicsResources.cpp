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

#include "GLResource.h"
#include "FontTexture.h"
#include "NuxGraphicsResources.h"
#include "GLResourceManager.h"

namespace nux
{

  NUX_IMPLEMENT_GLOBAL_OBJECT (NuxGraphicsResources);

  void NuxGraphicsResources::Constructor()
  {

  }

  void NuxGraphicsResources::Destructor()
  {

  }

  void NuxGraphicsResources::InitializeResources()
  {
//         FilePath m_FilePath;
//         m_FilePath.AddSearchPath(TEXT("")); // for case where fully qualified path is given
//         m_FilePath.AddSearchPath(TEXT("./ini"));
//         m_FilePath.AddSearchPath(TEXT("../ini"));
//         m_FilePath.AddSearchPath(TEXT("../../ini"));
//         m_FilePath.AddSearchPath(TEXT("../../../ini"));
//
//
//         NString file_search = TEXT("inalogic.ini");
//         NString FileName = m_FilePath.GetFile(file_search.GetTCharPtr());
//         if (FileName == TEXT(""))
//         {
//             nuxCriticalMsg(TEXT("[GraphicsDisplay::GraphicsDisplay] Can't find inalogic.ini file."));
// //             exit(-1);
//         }
//
//         if(FileName != TEXT(""))
//         {
//             NString key_name = TEXT("UseBackupFBO");
//             NString section_name = TEXT("OpenGLSystem");
//
//             if(CIniFile::RecordExists(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr()) == false)
//             {
//                 nuxDebugMsg(TEXT("[GraphicsDisplay::GraphicsDisplay] Key [%s] does not exit in .ini file."));
//             }
//             else
//             {
//                 bool m_BackupFrameBuffer;
//                 NString value = CIniFile::GetValue(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr());
//                 if((value == TEXT("1")) || (value == TEXT("true")))
//                 {
//                     m_BackupFrameBuffer = true;
//                 }
//                 else
//                 {
//                     m_BackupFrameBuffer = false;
//                 }
//             }
//
//             section_name = TEXT("SearchPath");
//             key_name = TEXT("FontPath");
//             if(CIniFile::RecordExists(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr()))
//             {
//                 NString FontPath = CIniFile::GetValue(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr());
//                 FontPath.ParseToArray(m_FontSearchPath, TEXT(";"));
//             }
//             else
//             {
//                 nuxCriticalMsg(TEXT("[GraphicsDisplay::GraphicsDisplay] Failed to read font search path from .ini file."));
// //                 return;
//
//             }
//
//             key_name = TEXT("UITexturePath");
//             if(CIniFile::RecordExists(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr()))
//             {
//                 NString UITexturePath = CIniFile::GetValue(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr());
//                 UITexturePath.ParseToArray(m_UITextureSearchPath, TEXT(";"));
//             }
//             else
//             {
//                 nuxCriticalMsg(TEXT("[GraphicsDisplay::GraphicsDisplay] Failed to read texture search path from .ini file."));
// //                 return;
//             }
//
//             key_name = TEXT("ShaderPath");
//             if(CIniFile::RecordExists(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr()))
//             {
//                 NString ShaderPath = CIniFile::GetValue(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr());
//                 ShaderPath.ParseToArray(m_ShaderSearchPath, TEXT(";"));
//             }
//             else
//             {
//                 nuxCriticalMsg(TEXT("[GraphicsDisplay::GraphicsDisplay] Failed to read shader search path from .ini file."));
// //                 return;
//             }
//         }

#ifdef NUX_OS_LINUX
    m_ResourcePathLocation.AddSearchPath (TEXT (PKGDATADIR) );
    m_ResourcePathLocation.AddSearchPath (TEXT (PKGDATADIR"/Fonts") );
    m_ResourcePathLocation.AddSearchPath (TEXT (PKGDATADIR"/Meshes") );
    m_ResourcePathLocation.AddSearchPath (TEXT (PKGDATADIR"/UITextures") );

#ifdef NUX_DEBUG
    /* For examples */
    m_ResourcePathLocation.AddSearchPath (TEXT ("../data") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../data/Fonts") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../data/Meshes") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../data/UITextures") );
#endif

#else
    m_ResourcePathLocation.AddSearchPath (TEXT ("") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("./") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../../") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("./Data") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../Data") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../../Data") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../../../Data") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("./Data/Fonts") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../Data/Fonts") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../../Data/Fonts") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../../../Data/Fonts") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("./Data/UITextures") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../Data/UITextures") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../../Data/UITextures") );
    m_ResourcePathLocation.AddSearchPath (TEXT ("../../../Data/UITextures") );
#endif

    m_ResourcePathLocation.AddSearchPath (m_FontSearchPath);
    m_ResourcePathLocation.AddSearchPath (m_ShaderSearchPath);
    m_ResourcePathLocation.AddSearchPath (m_UITextureSearchPath);
  }

#ifdef NUX_OS_WINDOWS
  NString NuxGraphicsResources::FindResourceLocation (const TCHAR *ResourceFileName, bool ErrorOnFail)
  {
    NString path = m_ResourcePathLocation.GetFile (ResourceFileName);

    if (path == TEXT ("") && ErrorOnFail)
    {
      nuxCriticalMsg (TEXT ("[NuxGraphicsResources::FindResourceLocation] Failed to locate resource file: %s."), ResourceFileName);
      return NString (TEXT(""));
    }

    return path;
  }

  NString NuxGraphicsResources::FindUITextureLocation (const TCHAR *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath (m_UITextureSearchPath);
    NString path = searchpath.GetFile (ResourceFileName);

    if ( (path == TEXT ("") ) && ErrorOnFail)
    {
      nuxCriticalMsg (TEXT ("[NuxGraphicsResources::FindResourceLocation] Failed to locate ui texture file: %s."), ResourceFileName);
      return NString (TEXT(""));
    }

    return path;
  }

  NString NuxGraphicsResources::FindShaderLocation (const TCHAR *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath (m_ShaderSearchPath);
    NString path = searchpath.GetFile (ResourceFileName);

    if ( (path == TEXT ("") ) && ErrorOnFail)
    {
      nuxCriticalMsg (TEXT ("[NuxGraphicsResources::FindResourceLocation] Failed to locate shader file: %s."), ResourceFileName);
      return NString (TEXT(""));
    }

    return path;
  }

  NString NuxGraphicsResources::FindFontLocation (const TCHAR *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath (m_FontSearchPath);
    NString path = searchpath.GetFile (ResourceFileName);

    if ( (path == TEXT ("") ) && ErrorOnFail)
    {
      nuxCriticalMsg (TEXT ("[NuxGraphicsResources::FindResourceLocation] Failed to locate font file file: %s."), ResourceFileName);
      return NString (TEXT(""));
    }

    return path;
  }
#elif defined(NUX_OS_LINUX)
  NString NuxGraphicsResources::FindResourceLocation (const TCHAR *ResourceFileName, bool ErrorOnFail)
  {
    NString path = m_ResourcePathLocation.GetFile (ResourceFileName);

    if (path == TEXT ("") && ErrorOnFail)
    {
      nuxCriticalMsg (TEXT ("[NuxGraphicsResources::FindResourceLocation] Failed to locate resource file: %s."), ResourceFileName);
      return NString (TEXT(""));
    }

    return path;
  }

  NString NuxGraphicsResources::FindUITextureLocation (const TCHAR *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath (m_UITextureSearchPath);
    NString path = searchpath.GetFile (ResourceFileName);

    if ( (path == TEXT ("") ) && ErrorOnFail)
    {
      nuxCriticalMsg (TEXT ("[NuxGraphicsResources::FindResourceLocation] Failed to locate ui texture file: %s."), ResourceFileName);
      return NString (TEXT(""));
    }

    return path;
  }

  NString NuxGraphicsResources::FindShaderLocation (const TCHAR *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath (m_ShaderSearchPath);
    NString path = searchpath.GetFile (ResourceFileName);

    if ( (path == TEXT ("") ) && ErrorOnFail)
    {
      nuxCriticalMsg (TEXT ("[NuxGraphicsResources::FindResourceLocation] Failed to locate shader file: %s."), ResourceFileName);
      return NString (TEXT(""));
    }

    return path;
  }

  NString NuxGraphicsResources::FindFontLocation (const TCHAR *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath (m_FontSearchPath);
    NString path = searchpath.GetFile (ResourceFileName);

    if ( (path == TEXT ("") ) && ErrorOnFail)
    {
      nuxCriticalMsg (TEXT ("[NuxGraphicsResources::FindResourceLocation] Failed to locate font file file: %s."), ResourceFileName);
      return NString (TEXT(""));
    }

    return path;
  }
#endif

}
