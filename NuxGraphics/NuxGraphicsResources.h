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

//         const std::vector<NString>& GetFontSearchPath() const {return m_FontSearchPath;}
//         const std::vector<NString>& GetShaderSearchPath() const {return m_ShaderSearchPath;}
//         const std::vector<NString>& GetUITextureSearchPath() const {return m_UITextureSearchPath;}

    IntrusiveSP<FontTexture> GetFont();
    IntrusiveSP<FontTexture> GetBoldFont();

    /*!
        Cache font textures into the provided resource cache.

    */
    void CacheFontTextures (NResourceCache &RsrcCache);

  private:
    IntrusiveSP<FontTexture> m_normal_font;    //!< The normal font renderer
    IntrusiveSP<FontTexture> m_bold_font;      //!< The bold font renderer

    std::vector<NString> m_FontSearchPath;
    std::vector<NString> m_ShaderSearchPath;
    std::vector<NString> m_UITextureSearchPath;
    FilePath m_ResourcePathLocation;
  };

}

#define GNuxGraphicsResources NUX_GLOBAL_OBJECT_INSTANCE(nux::NuxGraphicsResources)

#endif // NUXGRAPHICSRESOURCES_H
