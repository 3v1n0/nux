/*
 * Copyright 2010 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of either or both of the following licenses:
 *
 * 1) the GNU Lesser General Public License version 3, as published by the
 * Free Software Foundation; and/or
 * 2) the GNU Lesser General Public License version 2.1, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 and version 2.1 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Mirco Müller <mirco.mueller@canonical.com
 */

#ifndef STATICCAIROTEXT_H
#define STATICCAIROTEXT_H

#include "Nux/Nux.h"
#include "Nux/View.h"
#include "NuxGraphics/OpenGLEngine.h"
#include "Nux/TextureArea.h"
#include "NuxImage/CairoGraphics.h"

#include <pango/pango.h>
#include <pango/pangocairo.h>

#if defined(NUX_OS_LINUX)
#include <X11/Xlib.h>
#endif

namespace nux
{
  enum eFontStyle
  {
    eNormalStyle  = PANGO_STYLE_NORMAL,
    eObliqueStyle = PANGO_STYLE_OBLIQUE,
    eItalicStyle  = PANGO_STYLE_ITALIC,
  };

  enum eFontWeight
  {
    eLightWeight  = PANGO_WEIGHT_LIGHT,
    eNormalWeight = PANGO_WEIGHT_NORMAL,
    eBoldWeight   = PANGO_WEIGHT_BOLD,
  };

  class Validator;

  class StaticCairoText : public View
  {
    public:
      StaticCairoText (const TCHAR* text, NUX_FILE_LINE_PROTO);

      StaticCairoText (const TCHAR*     text,
                       nux::NString     fontName,
                       float            fontSize,
                       nux::eFontStyle  fontStyle,
                       nux::eFontWeight fontWeight,
                       nux::Color       textColor,
                       NUX_FILE_LINE_PROTO);

      ~StaticCairoText ();

      void PreLayoutManagement ();

      long PostLayoutManagement (long layoutResult);

      long ProcessEvent (IEvent& event,
        long    traverseInfo,
        long    processEventInfo);

      void Draw (GraphicsEngine& gfxContext,
            bool             forceDraw);

      void DrawContent (GraphicsEngine& gfxContext,
                   bool             forceDraw);

      void PostDraw (GraphicsEngine& gfxContext,
                bool             forceDraw);

      // public API
      void SetText (nux::NString text);

      void SetFontName (nux::NString fontName);

      void SetFontSize (float fontSize);

      void SetFontWeight (nux::eFontWeight fontWeight);

      void SetFontStyle (nux::eFontStyle fontStyle);

      void SetTextColor (nux::Color textColor);

      void GetTextExtents (int &width, int &height);

      sigc::signal<void, StaticCairoText&> sigTextChanged;

    protected:
	  
    private:
      nux::NString          _text;
      nux::NString          _fontName;
      float                 _fontSize;
      nux::eFontStyle       _fontStyle;
      nux::eFontWeight      _fontWeight;
      nux::Color            _textColor;

      nux::CairoGraphics*   _cairoGraphics;
      nux::BaseTexture*     _texture2D;
      int                   _dpiX;
      int                   _dpiY;
      cairo_font_options_t* _fontOpts;

      int _pre_layout_width;
      int _pre_layout_height;

      void GetDPI ();

      void GetTextExtents (const TCHAR* font, int &width, int &height);
      void DrawText (cairo_t*   cr, int width, int height, Color color);

      void UpdateTexture ();
  };
}

#endif // STATICCAIROTEXT_H
