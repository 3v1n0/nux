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

#ifndef NUXGRAPHICSRESOURCES_H
#define NUXGRAPHICSRESOURCES_H

namespace nux
{

  class FontTexture;
  class NResourceCache;

  class NuxGraphicsResources
  {
    NUX_DECLARE_GLOBAL_OBJECT (NuxGraphicsResources, NuxGraphicsGlobalSingletonInitializer);
  public:

    void InitializeResources();
    NString FindResourceLocation (const TCHAR *ResourceFileName, bool ErrorOnFail = false);
    NString FindUITextureLocation (const TCHAR *ResourceFileName, bool ErrorOnFail = false);
    NString FindShaderLocation (const TCHAR *ResourceFileName, bool ErrorOnFail = false);
    NString FindFontLocation (const TCHAR *ResourceFileName, bool ErrorOnFail = false);

  private:

    std::vector<NString> m_FontSearchPath;
    std::vector<NString> m_ShaderSearchPath;
    std::vector<NString> m_UITextureSearchPath;
    FilePath m_ResourcePathLocation;
  };

}

#define GNuxGraphicsResources NUX_GLOBAL_OBJECT_INSTANCE(nux::NuxGraphicsResources)

#endif // NUXGRAPHICSRESOURCES_H
