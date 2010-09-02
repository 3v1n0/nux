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


#ifndef ABSTRACTPAINTLAYER_H
#define ABSTRACTPAINTLAYER_H

NAMESPACE_BEGIN_GUI

class AbstractPaintLayer
{
public:

    enum PaintLayerType
    {
        PaintLayer_Color,
        PaintLayer_Shape,
        PaintLayer_ShapeROP,
        PaintLayer_Texture,
        PaintLayer_DeviceTexture,
        PaintLayer_ImageStyle,
    };

    virtual ~AbstractPaintLayer() {};
    virtual AbstractPaintLayer* Clone() const = 0; //Virtual Constructor Idiom 

    virtual void Renderlayer(GraphicsContext& GfxContext) = 0;

    virtual void SetGeometry(const Geometry& geo) {m_geometry = geo;}
    Geometry GetGeometry() const {return m_geometry;}

protected:
    Geometry m_geometry;
};

NAMESPACE_END_GUI

#endif // ABSTRACTPAINTLAYER_H

