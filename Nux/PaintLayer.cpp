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


#include "Nux.h"
#include "NuxGraphics/OpenGLEngine.h"
#include "NuxGraphics/RenderingPipe.h"
#include "Utils.h"
#include "PaintLayer.h"

NAMESPACE_BEGIN_GUI

ColorLayer::ColorLayer(const Color& color, bool write_alpha, const ROPConfig& ROP)
{
    m_color = color;
    m_write_alpha = write_alpha;
    m_rop = ROP;
}

void ColorLayer::Renderlayer(GraphicsContext& GfxContext)
{
    GfxContext.GetRenderStates().SetBlend(m_rop.Blend, m_rop.SrcBlend, m_rop.DstBlend);
    GfxContext.QRP_GLSL_Color(m_geometry.x, m_geometry.y, m_geometry.GetWidth(), m_geometry.GetHeight(), m_color);
    if(m_rop.Blend)
        GfxContext.GetRenderStates().SetBlend(false);
}

AbstractPaintLayer* ColorLayer::Clone() const
{
    return new ColorLayer(*this);
}

/////////////////////////////////////////////////////
ShapeLayer::ShapeLayer(UXStyleImageRef image_style, const Color& color, unsigned long corners, bool write_alpha, const ROPConfig& ROP)
{
    m_image_style = image_style;
    m_color = color;
    m_write_alpha = write_alpha;
    m_rop = ROP;
    m_rop.Blend = true;
    m_corners = corners;
}

void ShapeLayer::Renderlayer(GraphicsContext& GfxContext)
{
    GetThreadPainter().PaintShapeCornerROP(GfxContext, m_geometry, m_color, m_image_style, m_corners, m_write_alpha, m_rop);
    if(m_rop.Blend)
        GfxContext.GetRenderStates().SetBlend(false);
}

AbstractPaintLayer* ShapeLayer::Clone() const
{
    return new ShapeLayer(*this);
}

/////////////////////////////////////////////////////
SliceScaledTextureLayer::SliceScaledTextureLayer(UXStyleImageRef image_style, const Color& color, unsigned long corners, bool write_alpha, const ROPConfig& ROP)
{
    m_image_style = image_style;
    m_color = color;
    m_write_alpha = write_alpha;
    m_rop = ROP;
    m_corners = corners;
}

void SliceScaledTextureLayer::Renderlayer(GraphicsContext& GfxContext)
{
    GetThreadPainter().PaintTextureShape(GfxContext, m_geometry, m_image_style);
}

AbstractPaintLayer* SliceScaledTextureLayer::Clone() const
{
    return new SliceScaledTextureLayer(*this);
}

/////////////////////////////////////////////////////
TextureLayer::TextureLayer(TRefGL<IOpenGLBaseTexture> device_texture, TexCoordXForm texxform, const Color& color, bool write_alpha, const ROPConfig& ROP)
{
    m_device_texture = device_texture;
    m_color = color;
    m_write_alpha = write_alpha;
    m_rop = ROP;
    m_texxform = texxform;
}

void TextureLayer::Renderlayer(GraphicsContext& GfxContext)
{
    GfxContext.QRP_GLSL_1Tex(m_geometry.x, m_geometry.y, m_geometry.GetWidth(), m_geometry.GetHeight(), m_device_texture, 
        m_texxform, m_color);
}

AbstractPaintLayer* TextureLayer::Clone() const
{
    return new TextureLayer(*this);
}


NAMESPACE_END_GUI
