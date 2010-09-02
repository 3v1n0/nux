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


#ifndef PAINTLAYER_H
#define PAINTLAYER_H

#include "AbstractPaintLayer.h"
#include "NuxGraphics/OpenGLEngine.h"

NAMESPACE_BEGIN_GUI

class NTexture;

class ColorLayer: public AbstractPaintLayer
{
public:
    ColorLayer(const Color& color, bool WriteAlpha = false, const ROPConfig& ROP = ROPConfig::Default);

    void SetColor(const Color& color);
    Color GetColor() const;
    virtual void Renderlayer(GraphicsContext& GfxContext);
    virtual AbstractPaintLayer* Clone() const;

private:
    Color m_color;
    bool m_write_alpha;
    ROPConfig m_rop;
};

class ShapeLayer: public AbstractPaintLayer
{
public:
    ShapeLayer(UXStyleImageRef imageStyle, const Color& color, unsigned long Corners = eAllCorners, bool WriteAlpha = false, const ROPConfig& ROP = ROPConfig::Default);
    virtual void Renderlayer(GraphicsContext& GfxContext);
    virtual AbstractPaintLayer* Clone() const;

private:
    UXStyleImageRef m_image_style;
    Color m_color;
    bool m_write_alpha;
    ROPConfig m_rop;
    unsigned long m_corners;
};

class SliceScaledTextureLayer: public AbstractPaintLayer
{
public:
    SliceScaledTextureLayer(UXStyleImageRef imageStyle, const Color& color, unsigned long Corners = eAllCorners, bool WriteAlpha = false, const ROPConfig& ROP = ROPConfig::Default);
    virtual void Renderlayer(GraphicsContext& GfxContext);
    virtual AbstractPaintLayer* Clone() const;

private:
    UXStyleImageRef m_image_style;
    Color m_color;
    bool m_write_alpha;
    ROPConfig m_rop;
    unsigned long m_corners;
};

class TextureLayer: public AbstractPaintLayer
{
public:
    TextureLayer(TRefGL< IOpenGLBaseTexture > device_texture, TexCoordXForm texxform, const Color& color, bool WriteAlpha = false, const ROPConfig& ROP = ROPConfig::Default);
    virtual void Renderlayer(GraphicsContext& GfxContext);
    virtual AbstractPaintLayer* Clone() const;

private:
    TRefGL< IOpenGLBaseTexture > m_device_texture;
    Color m_color;
    bool m_write_alpha;
    ROPConfig m_rop;
    TexCoordXForm m_texxform;
};

NAMESPACE_END_GUI

#endif // PAINTLAYER_H