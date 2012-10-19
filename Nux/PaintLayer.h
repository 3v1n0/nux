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


#ifndef PAINTLAYER_H
#define PAINTLAYER_H

#include "AbstractPaintLayer.h"
#include "NuxGraphics/GraphicsEngine.h"

namespace nux
{

  class BaseTexture;

  class ColorLayer: public AbstractPaintLayer
  {
  public:
    ColorLayer(const Color& color, bool WriteAlpha = false, const ROPConfig& ROP = ROPConfig::Default);

    void SetColor(const Color& color);
    Color GetColor() const;
    virtual void Renderlayer(GraphicsEngine& graphics_engine);
    virtual AbstractPaintLayer* Clone() const;

  private:
    Color _color;
    bool m_write_alpha;
    ROPConfig m_rop;
  };

  class ShapeLayer: public AbstractPaintLayer
  {
  public:
    ShapeLayer(UXStyleImageRef imageStyle, const Color& color, unsigned long Corners = eAllCorners, bool WriteAlpha = false, const ROPConfig& ROP = ROPConfig::Default);
    virtual void Renderlayer(GraphicsEngine& graphics_engine);
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
    virtual void Renderlayer(GraphicsEngine& graphics_engine);
    virtual AbstractPaintLayer* Clone() const;

  private:
    UXStyleImageRef m_image_style;
    Color m_color;
    bool m_write_alpha;
    ROPConfig m_rop;
    unsigned long m_corners;
  };
  
  class CompositionLayer: public AbstractPaintLayer
  {
  public:
    //! Layer blend operation.
    /*!
        Blend(texture0*color0, texture1*color1);
    */
    CompositionLayer(ObjectPtr <IOpenGLBaseTexture> texture0, TexCoordXForm texxform0, const Color& color0,
		     ObjectPtr <IOpenGLBaseTexture> texture1, TexCoordXForm texxform1, const Color& color1,
		     LayerBlendMode layer_blend_mode,
		     bool write_alpha, const ROPConfig& ROP);

    //! Layer blend operation.
    /*!
        Blend(texture0*color0, blend_color);
    */
    CompositionLayer(ObjectPtr <IOpenGLBaseTexture> texture0, TexCoordXForm texxform0, const Color& color0,
		     const Color& blend_color, LayerBlendMode layer_blend_mode,
		     bool write_alpha, const ROPConfig& ROP);

    //! Layer blend operation.
    /*!
        Blend(texture0*color0, color0);
    */
    CompositionLayer(const Color& base_color, ObjectPtr <IOpenGLBaseTexture> texture0, 
		     TexCoordXForm texxform0, const Color& color0,
		     LayerBlendMode layer_blend_mode,
		     bool write_alpha, const ROPConfig& ROP);

    CompositionLayer(const Color& base_color, const Color& blend_color, LayerBlendMode layer_blend_mode,
         bool write_alpha, const ROPConfig& ROP);
    
    virtual ~CompositionLayer();
    
    virtual void Renderlayer(GraphicsEngine& graphics_engine);
    
    virtual AbstractPaintLayer* Clone() const;

  private:
    ObjectPtr <IOpenGLBaseTexture > m_source_texture;
    Color m_source_texture_color;
    TexCoordXForm m_source_texture_texxform;
    
    ObjectPtr <IOpenGLBaseTexture> m_foreground_texture;
    Color m_foreground_texture_color;
    TexCoordXForm m_foreground_texture_texxform;
    
    Color m_source_color;
    Color m_foreground_color;
    
    bool m_write_alpha;
    ROPConfig m_rop;

    LayerBlendMode m_blend_mode;
  };

  class TextureLayer: public AbstractPaintLayer
  {
  public:
    TextureLayer(ObjectPtr< IOpenGLBaseTexture > device_texture, TexCoordXForm texxform, const Color& color, bool WriteAlpha = false, const ROPConfig& ROP = ROPConfig::Default);
    TextureLayer(ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm texxform, const Color& color0,
		 bool write_alpha, const ROPConfig& ROP, const Color& blend_color, LayerBlendMode color_blend_mode);
    virtual ~TextureLayer();
    virtual void Renderlayer(GraphicsEngine& graphics_engine);
    virtual AbstractPaintLayer* Clone() const;

    virtual ObjectPtr< IOpenGLBaseTexture> GetDeviceTexture();

  private:
    ObjectPtr< IOpenGLBaseTexture > m_device_texture;
    Color m_color;
    bool m_write_alpha;
    ROPConfig m_rop;
    TexCoordXForm m_texxform;
    
    Color m_blend_color;
    LayerBlendMode m_color_blend_mode;
  };

}

#endif // PAINTLAYER_H
