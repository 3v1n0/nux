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


#ifndef CAIROGRAPHICS_H
#define CAIROGRAPHICS_H

#include "cairo.h"
#include "BitmapFormats.h"
#include "ImageSurface.h"

NAMESPACE_BEGIN

//! A cairo graphics container
/*!
    CairoGraphics encapsulates a cairo surface and context.
*/
class CairoGraphics
{
public:
    CairoGraphics(cairo_format_t format, int width, int height);
    ~CairoGraphics();

    //! Return a cairo context to the encapsulated surface.
    /*!
        @return A cairo context.
    */
    cairo_t* GetContext();
    //! Create a NBitmapData pointer to a 2D texture data.
    /*!
        The returned data must be destroyed with delete.
        @return A pointer to a 2D texture data.
    */
    NBitmapData* GetBitmap();

private:
    //! Cairo surface format
    cairo_format_t m_surface_format;
    //! Cairo surface
    cairo_surface_t* m_cairo_surface;
    int m_width; //!< Surface width.
    int m_height; //!< Surface height.
};

NAMESPACE_END

#endif // CAIROGRAPHICS_H
