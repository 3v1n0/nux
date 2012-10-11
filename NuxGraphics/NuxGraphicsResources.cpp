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
  std::string NuxGraphicsResources::FindResourceLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    std::string path = m_ResourcePathLocation.GetFile(ResourceFileName);

    if (path == "" && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate resource file: %s.", ResourceFileName);
      return std::string("");
    }

    return path;
  }

  std::string NuxGraphicsResources::FindUITextureLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_UITextureSearchPath);
    std::string path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate ui texture file: %s.", ResourceFileName);
      return std::string("");
    }

    return path;
  }

  std::string NuxGraphicsResources::FindShaderLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_ShaderSearchPath);
    std::string path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate shader file: %s.", ResourceFileName);
      return std::string("");
    }

    return path;
  }

  std::string NuxGraphicsResources::FindFontLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_FontSearchPath);
    std::string path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate font file file: %s.", ResourceFileName);
      return std::string("");
    }

    return path;
  }
#elif defined(NUX_OS_LINUX)
  std::string NuxGraphicsResources::FindResourceLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    std::string path = m_ResourcePathLocation.GetFile(ResourceFileName);

    if (path == "" && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate resource file: %s.", ResourceFileName);
      return "";
    }

    return path;
  }

  std::string NuxGraphicsResources::FindUITextureLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_UITextureSearchPath);
    std::string path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate ui texture file: %s.", ResourceFileName);
      return std::string("");
    }

    return path;
  }

  std::string NuxGraphicsResources::FindShaderLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_ShaderSearchPath);
    std::string path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate shader file: %s.", ResourceFileName);
      return std::string("");
    }

    return path;
  }

  std::string NuxGraphicsResources::FindFontLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_FontSearchPath);
    std::string path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[NuxGraphicsResources::FindResourceLocation] Failed to locate font file file: %s.", ResourceFileName);
      return std::string("");
    }

    return path;
  }
#endif

}
