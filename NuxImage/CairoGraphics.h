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


#ifndef CAIROGRAPHICS_H
#define CAIROGRAPHICS_H

#include "cairo/cairo.h"
#include "BitmapFormats.h"
#include "ImageSurface.h"

#include <stack>

namespace nux
{


//! A cairo graphics container
  /*!
      CairoGraphics encapsulates a cairo surface and context.
  */
  class CairoGraphics
  {
  public:
    CairoGraphics (cairo_format_t format, int width, int height);
    ~CairoGraphics();

    //! Return a cairo context to the encapsulated surface.
    /*!
        Return the cairo context of this object. Call cairo_destroy to destroy the context when you are done with it.
        @return A cairo context.
    */
    cairo_t *GetContext();

    cairo_surface_t* GetSurface ();
    //! Create a NBitmapData pointer to a 2D texture data.
    /*!
        The returned data must be destroyed with delete.
        @return A pointer to a 2D texture data.
    */
    NBitmapData *GetBitmap();

    int GetWidth () const;
    int GetHeight () const;

    bool PushState ();
    bool PopState ();

    bool ClearCanvas();

    bool ClearRect(double x, double y, double w, double h);

    bool DrawLine(double x0, double y0, double x1, double y1, double width, const Color &c);

    bool DrawFilledRect(double x, double y, double w, double h, const Color &c);

    bool DrawCanvas(double x, double y, CairoGraphics *cg);

    bool IntersectRectClipRegion(double x, double y, double w, double h);

    bool IntersectGeneralClipRegion(std::list<Rect> &region);

  /**
   * Enum used to specify horizontal alignment.
   */
  enum Alignment {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT,
    ALIGN_JUSTIFY
  };

  /**
   * Enum used to specify vertical alignment.
   */
  enum VAlignment {
    VALIGN_TOP,
    VALIGN_MIDDLE,
    VALIGN_BOTTOM
  };

  /**
   * Enum used to specify trimming type.
   */
  enum Trimming {
    TRIMMING_NONE,
    TRIMMING_CHARACTER,
    TRIMMING_WORD,
    TRIMMING_CHARACTER_ELLIPSIS,
    TRIMMING_WORD_ELLIPSIS,
    TRIMMING_PATH_ELLIPSIS
  };

  /**
   * Enum used to specify text flags.
   */
  enum TextFlag {
    TEXT_FLAGS_NONE = 0,
    TEXT_FLAGS_UNDERLINE = 1,
    TEXT_FLAGS_STRIKEOUT = 2,
    TEXT_FLAGS_WORDWRAP = 4
  };

  private:
    //! Cairo surface format
    cairo_format_t m_surface_format;
    //! Cairo surface
    cairo_surface_t *_cairo_surface;

    cairo_t * _cr;
    int _width; //!< Surface width.
    int _height; //!< Surface height.

    double _zoom;
    float _opacity;
    std::stack<float> _opacity_stack;
  };

}

#endif // CAIROGRAPHICS_H

