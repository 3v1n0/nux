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


#ifndef RENDERINGPIPE_H
#define RENDERINGPIPE_H

#include "GLResource.h"

namespace nux { //NUX_NAMESPACE_BEGIN

//! Graphics wraping modes
typedef enum
{
    TEXWRAP_UNKNOWN = 0,
    TEXWRAP_REPEAT,
    TEXWRAP_CLAMP,
    TEXWRAP_CLAMP_TO_EDGE,
    TEXWRAP_CLAMP_TO_BORDER,
    TEXWRAP_MIRRORED_REPEAT,
    TEXWRAP_MIRROR_CLAMP_EXT,
    TEXWRAP_MIRROR_CLAMP_TO_EDGE_EXT,
    TEXWRAP_MIRROR_CLAMP_TO_BORDER_EXT,
} TexWrap;

//! Graphics filtering modes
typedef enum
{
    TEXFILTER_UNKNOWN = 0,
    TEXFILTER_LINEAR,
    TEXFILTER_NEAREST,
    TEXFILTER_NEAREST_MIPMAP_NEAREST,
    TEXFILTER_LINEAR_MIPMAP_NEAREST,
    TEXFILTER_NEAREST_MIPMAP_LINEAR,
    TEXFILTER_LINEAR_MIPMAP_LINEAR,
} TexFilter;

//! Graphics blend modes
typedef enum
{
    ROPBLEND_UNKNOWN = 0,
    ROPBLEND_ZERO,
    ROPBLEND_ONE,
    ROPBLEND_SRC_COLOR,
    ROPBLEND_ONE_MINUS_SRC_COLOR,
    ROPBLEND_DST_COLOR,
    ROPBLEND_ONE_MINUS_DST_COLOR,
    ROPBLEND_SRC_ALPHA,
    ROPBLEND_ONE_MINUS_SRC_ALPHA,
    ROPBLEND_DST_ALPHA,
    ROPBLEND_ONE_MINUS_DST_ALPHA,
    ROPBLEND_CONSTANT_COLOR,
    ROPBLEND_ONE_MINUS_CONSTANT_COLOR,
    ROPBLEND_CONSTANT_ALPHA,
    ROPBLEND_ONE_MINUS_CONSTANT_ALPHA,
    ROPBLEND_SRC_ALPHA_SATURATE,
} RopBlend;

//! Texture parameters and coordinates computation class
class TexCoordXForm
{
public:
    //! Texture coordinates computation mode
    typedef enum
    {
        OFFSET_SCALE_COORD, //!< Texture coordinates are offset and scaled 
        OFFSET_COORD,       //!< Textures coordinates are offset only
        NORMALIZED_COORD,   //!< Provided normalized texture coordinates in u0, v0, u1, v1
        UNNORMALIZED_COORD, //!< Provided un-normalized texture coordinates in u0, v0, u1, v1
        FIXED_COORD,        //!< Provided fix coordinates in u0, v0, u1, v1
    } TexCoordType;

    TexCoordXForm();

    void SetTexCoordType(TexCoordType tex_coord_type);
    void FlipUCoord(bool b);
    void FlipVCoord(bool b);
    void FlipUVCoord(bool flip_u, bool flip_v);
    void SetFilter(TexFilter min_fliter, TexFilter mag_filter);
    void SetWrap(TexWrap u_wrap, TexWrap v_wrap);

    //! Texture coordinates are computed automatically by following the TexCoordType policy or provided by the user
    float u0, v0, u1, v1;
    float uscale;
    float vscale;
    float uoffset;
    float voffset;
    TexWrap uwrap;
    TexWrap vwrap;
    TexFilter min_filter;
    TexFilter mag_filter;
    bool flip_u_coord;
    bool flip_v_coord;
    TexCoordType m_tex_coord_type;
};

//! Compute texture coordinates and wrapping mode according to TexCoordXForm
/*!
    @param quad_width   Quad width.
    @param quad_height  Quad height.
    @param tex          Device texture.
    @param texxform     Texture coordinates computation mode.
*/
void QRP_Compute_Texture_Coord(t_int32 quad_width, t_int32 quad_height, TRefGL<IOpenGLBaseTexture> tex, TexCoordXForm& texxform);

} //NUX_NAMESPACE_END

#endif // RENDERINGPIPE_H
