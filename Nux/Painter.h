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


#ifndef PAINTER_H
#define PAINTER_H

#include <string>
#include <iostream>
#include <list>

#include "Utils.h"
#include "NuxGraphics/GraphicsEngine.h"
#include "PaintLayer.h"

namespace nux
{

  class TextLineRenderer;
  class BaseTexture;

  class GeometryPositioning
  {
  public:
    GeometryPositioning();
    GeometryPositioning(
      HorizontalAlignment horizontal_aligment,
      VerticalAlignment vertical_aligment,
      bool stretch_horizontal = FALSE,
      bool stretch_vertical = FALSE,
      int horizontal_margin = 0,
      int vertical_margin = 0);
    ~GeometryPositioning();

    bool m_stretch_horizontal;  // if TRUE, the content width will be stretched over the width of the container.
    bool m_stretch_vertical;    // if TRUE, the content height will be stretched over the height of the container.

    void SetAlignment(HorizontalAlignment ha, VerticalAlignment va)
    {
      m_vertical_aligment = va;
      m_horizontal_aligment = ha;
    }

    VerticalAlignment m_vertical_aligment;
    HorizontalAlignment m_horizontal_aligment;

    int m_horizontal_margin;    // Add a margin to the horizontal alignment
    int m_vertical_margin;      // Add a margin to the vertical alignment
  };

//! Compute the position of one geometry inside an other.
  /*!
      Compute the position of one geometry inside an other.

      @container_geo The container geometry
      @content_geo   The content geometry
      @GeometryPositioning    The parameter to use to compute the position of the content.
      @return the computed geometry.
  */
  Geometry ComputeGeometryPositioning(const Geometry &container_geo, const Geometry &content_geo, GeometryPositioning gctx);

//! State of UI controls
  /*
      Class storing the states of User interface controls such button, radio, checkbox, combobox.
  */
  class InteractState
  {
  public:

    /*
       Default constructor. All states are set to false.
    */
    InteractState();
    InteractState(bool on, bool focus, bool prelight, bool disable);
    ~InteractState();

    bool is_on;
    bool is_focus;
    bool is_prelight;
    bool is_disable;
  };




// enum PainterBackgroundType
// {
//     eColorBackground = 0,
//     eShapeBackground,
//     eShapeCornerBackground,
//     eTextureBackground,
// };

  class TextureAlignmentStyle
  {
  public:
    TextureAlignmentStyle()
    {
      horz_alignment = eTACenter;
      vert_alignment = eTACenter;
      horizontal_margin = 0;
      vertical_margin = 0;
    };
    TextureAlignmentStyle(TextureAlignment horz_align, TextureAlignment vert_align)
    {
      horz_alignment = horz_align;
      vert_alignment = vert_align;
      horizontal_margin = 0;
      vertical_margin = 0;
    };

    ~TextureAlignmentStyle() {};

    enum TextureAlignment horz_alignment;
    enum TextureAlignment vert_alignment;
    int horizontal_margin;
    int vertical_margin;
  };

  class BasePainter
  {
  public:
    BasePainter(WindowThread *window_thread);
    virtual ~BasePainter();
    int intTest;

    //! Draw unscaled texture at position(x, y). The size of the quad is the size of the texture.
    virtual void Draw2DTexture(GraphicsEngine &graphics_engine, BaseTexture *Texture, int x, int y) const;
    virtual void Draw2DTextureAligned(GraphicsEngine &graphics_engine, BaseTexture *Texture, const Geometry &g, TextureAlignmentStyle tex_align) const;

    ///////////////////

    ////////////////////
    // TEXT PAINTING  //
    ////////////////////
    virtual int PaintColorTextLineEdit(GraphicsEngine &graphics_engine, const Geometry &g,
                                       std::string const& Str,
                                        Color TextColor,
                                        bool WriteAlphaChannel,
                                        Color SelectedTextColor,
                                        Color SelectedTextBackgroundColor,
                                        Color TextBlinkColor,
                                        Color CursorColor,
                                        bool ShowCursor, unsigned int CursorPosition,
                                        int offset = 0,
                                        int selection_start = 0, int selection_end = 0) const;

    virtual int PaintTextLineStatic(GraphicsEngine &graphics_engine,
                                    ObjectPtr<FontTexture> Font,
                                    Geometry const& g,
                                    std::string const& text_line,
                                    Color const& color = Color(0.0f, 0.0f, 0.0f, 1.0f),
                                    bool WriteAlphaChannel = true,
                                    TextAlignment alignment = eAlignTextLeft) const;

    void Paint2DQuadColor(GraphicsEngine &graphics_engine, const Geometry &g, const Color &c0) const;
    void Paint2DQuadColor(GraphicsEngine &graphics_engine, const Geometry &g, const Color &c0_top_left, const Color &c1_bottom_left, const Color &c2_bottom_right, const Color &c3_top_right) const;

    void Paint2DQuadColor(GraphicsEngine &graphics_engine, int x, int y, int width, int height, const Color &c0) const;
    void Paint2DQuadColor(GraphicsEngine &graphics_engine, int x, int y, int width, int height, const Color &c0_top_left, const Color &c1_bottom_left, const Color &c2_bottom_right, const Color &c3_top_right) const;

    //! Paint a 2D quad with a gradient color going from Top to Bottom.
    /*! Paint a 2D quad with a gradient color going from Top to Bottom.
        @param g Geometry of the quad.
        @param TopColor color at the top of the quad.
        @param TopColor color at the bottom of the quad.
    */
    void Paint2DQuadVGradient(GraphicsEngine &graphics_engine, const Geometry &g, Color TopColor, Color BottomColor) const;
    //! Paint a 2D quad with a gradient color going from Left to Right.
    /*! Paint a 2D quad with a gradient color going from Left to Right.
        @param g Geometry of the quad.
        @param LeftColor color at the top of the quad.
        @param RightColor color at the bottom of the quad.
    */
    void Paint2DQuadHGradient(GraphicsEngine &graphics_engine, const Geometry &g, Color LeftColor, Color RightColor) const;

    void Paint2DQuadWireframe(GraphicsEngine &graphics_engine, const Geometry &g, Color c0) const;
    void Paint2DQuadWireframe(GraphicsEngine &graphics_engine, const Geometry &g, Color c_top_left, Color c_bottom_left, Color c_bottom_right, Color c_top_right) const;

    void Paint2DQuadWireframe(GraphicsEngine &graphics_engine, int x, int y, int width, int height, Color c0) const;
    void Paint2DQuadWireframe(GraphicsEngine &graphics_engine, int x, int y, int width, int height, Color c_top_left, Color c_bottom_left, Color c_bottom_right, Color c_top_right) const;

    void Draw2DTriangleColor(GraphicsEngine &graphics_engine, int x0, int y0,
                              int x1, int y1,
                              int x2, int y2,
                              Color c0);
    void Draw2DTriangleColor(GraphicsEngine &graphics_engine, int x0, int y0,
                              int x1, int y1,
                              int x2, int y2,
                              Color c0, Color c1, Color c2);

    //////////////////////
    // DRAW LINES       //
    //////////////////////
    void Draw2DLine(GraphicsEngine &graphics_engine, int x0, int y0,
                     int x1, int y1, Color c0) const;
    void Draw2DLine(GraphicsEngine &graphics_engine, int x0, int y0,
                     int x1, int y1, Color c0, Color c1) const;

    ////////////////////
    // Themes         //
    ////////////////////
    void PaintShape(GraphicsEngine &graphics_engine, const Geometry &geo, const Color &c0, UXStyleImageRef style, bool WriteAlpha = true) const;
    void PaintShapeCorner(GraphicsEngine &graphics_engine, const Geometry &geo, const Color &c0, UXStyleImageRef style, long corners, bool WriteAlpha = true) const;
    void PaintShapeROP(GraphicsEngine &graphics_engine, const Geometry &geo, const Color &c0, UXStyleImageRef style, bool WriteAlpha = true,
                        const ROPConfig &ROP = ROPConfig::Default) const;
    void PaintShapeCornerROP(GraphicsEngine &graphics_engine,
                            const Geometry &geo,
                            const Color &c0,
                            UXStyleImageRef style,
                            long corners,
                            bool WriteAlpha = true,
                            const ROPConfig &ROP = ROPConfig::Default) const;

    void PaintTextureShape(GraphicsEngine &graphics_engine, const Geometry &geo, UXStyleImageRef style) const;
    void PaintTextureShape(GraphicsEngine &graphics_engine, const Geometry &geo, BaseTexture *Texture,
                            int border_left, int border_right, int border_top, int border_bottom, bool draw_borders_only, bool premultiply = true) const;

    //! Draw Check Box.
    /*!
        Draw a Check box.
    */

    void PaintCheckBox(GraphicsEngine &graphics_engine, const Geometry &geo, const InteractState &interaction_state,
                        Color check_mark_color = Color(0x0), Color check_box_color = Color(0x0));

    void PaintRadioButton(GraphicsEngine &graphics_engine, const Geometry &geo, const InteractState &interaction_state,
                           Color check_mark_color = Color(0x0), Color check_box_color = Color(0x0));

    void PaintHorizontalGradientQuad(GraphicsEngine &graphics_engine, const Geometry &geo, int num_color, float *percentage_array, Color *color_array);

  public:
    void PushColorLayer(GraphicsEngine &graphics_engine, const Geometry &geo,
                         Color color,
                         bool WriteAlpha = false,
                         const ROPConfig &ROP = ROPConfig::Default);

    void PushShapeLayer(GraphicsEngine &graphics_engine, Geometry geo,
                         UXStyleImageRef imageStyle,
                         const Color &color,
                         unsigned long Corners = eAllCorners,
                         bool WriteAlpha = false,
                         const ROPConfig &ROP = ROPConfig::Default);

    void PushSliceScaledTextureLayer(GraphicsEngine &graphics_engine, Geometry geo,
                                      UXStyleImageRef imageStyle,
                                      const Color &color,
                                      unsigned long Corners = eAllCorners,
                                      bool WriteAlpha = false,
                                      const ROPConfig &ROP = ROPConfig::Default);

    void PushTextureLayer(GraphicsEngine &graphics_engine, Geometry geo,
                           ObjectPtr<IOpenGLBaseTexture> DeviceTexture,
                           TexCoordXForm texxform,
                           const Color &color,
                           bool WriteAlpha = false,
                           const ROPConfig &ROP = ROPConfig::Default);

    void PushColorizeTextureLayer(GraphicsEngine &graphics_engine, Geometry geo,
				  ObjectPtr<IOpenGLBaseTexture> DeviceTexture,
				  TexCoordXForm texxform,
				  const Color &color,
				  bool WriteAlpha,
				  const ROPConfig &ROP,
				  const Color &blend_color,
				  LayerBlendMode layer_blend_mode);

    void PushDrawColorLayer(GraphicsEngine &graphics_engine, const Geometry &geo,
                             Color color,
                             bool WriteAlpha = false,
                             const ROPConfig &ROP = ROPConfig::Default);

    void PushDrawShapeLayer(GraphicsEngine &graphics_engine, Geometry geo,
                             UXStyleImageRef imageStyle,
                             const Color &color,
                             unsigned long Corners = eAllCorners,
                             bool WriteAlpha = false,
                             const ROPConfig &ROP = ROPConfig::Default);

    void PushDrawSliceScaledTextureLayer(GraphicsEngine &graphics_engine, Geometry geo,
                                          UXStyleImageRef imageStyle,
                                          const Color &color,
                                          unsigned long Corners = eAllCorners,
                                          bool WriteAlpha = false,
                                          const ROPConfig &ROP = ROPConfig::Default);

    void PushDrawTextureLayer(GraphicsEngine &graphics_engine, Geometry geo,
                               ObjectPtr<IOpenGLBaseTexture> DeviceTexture,
                               TexCoordXForm texxform,
                               const Color &color,
                               bool WriteAlpha = false,
                               const ROPConfig &ROP = ROPConfig::Default);

    void PushDrawColorizeTextureLayer(GraphicsEngine& graphics_engine, Geometry geo,
				  ObjectPtr<IOpenGLBaseTexture> DeviceTexture,
				  TexCoordXForm texxform,
				  const Color &color,
				  bool WriteAlpha,
				  const ROPConfig &ROP,
				  const Color &blend_color,
				  LayerBlendMode layer_blend_mode);
    
    void PushCompositionLayer (GraphicsEngine &graphics_engine,
			       Geometry geo,
			       ObjectPtr <IOpenGLBaseTexture> texture0,
			       TexCoordXForm texxform0,
			       const Color &color0,
			       ObjectPtr <IOpenGLBaseTexture> texture1,
			       TexCoordXForm texxform1,
			       const Color &color1, 
			       LayerBlendMode layer_blend_mode,
			       bool WriteAlpha,
			       const ROPConfig &ROP);

    void PushDrawCompositionLayer (GraphicsEngine &graphics_engine,
			       Geometry geo,
			       ObjectPtr <IOpenGLBaseTexture> texture0,
			       TexCoordXForm texxform0,
			       const Color &color0,
			       ObjectPtr <IOpenGLBaseTexture> texture1,
			       TexCoordXForm texxform1,
			       const Color &color1, 
			       LayerBlendMode layer_blend_mode,
			       bool WriteAlpha,
			       const ROPConfig &ROP);

    void PushCompositionLayer (GraphicsEngine &graphics_engine,
			       Geometry geo,
			       ObjectPtr <IOpenGLBaseTexture> texture0,
			       TexCoordXForm texxform0,
			       const Color& color0,
			       const Color& blend_color,
			       LayerBlendMode layer_blend_mode,
			       bool WriteAlpha,
			       const ROPConfig &ROP);
    
    void PushDrawCompositionLayer (GraphicsEngine &graphics_engine,
				   Geometry geo,
				   ObjectPtr <IOpenGLBaseTexture> texture0,
				   TexCoordXForm texxform0,
				   const Color &color0,
				   const Color& blend_color,
				   LayerBlendMode layer_blend_mode,
				   bool WriteAlpha,
				   const ROPConfig &ROP);

    void PushCompositionLayer (GraphicsEngine &graphics_engine,
			       Geometry geo,
			       const Color& base_color,
			       ObjectPtr <IOpenGLBaseTexture> texture0,
			       TexCoordXForm texxform0,
			       const Color& color0,
			       LayerBlendMode layer_blend_mode,
			       bool WriteAlpha,
			       const ROPConfig &ROP);
    
    void PushDrawCompositionLayer (GraphicsEngine &graphics_engine,
				   Geometry geo,
				   const Color& base_color,
				   ObjectPtr <IOpenGLBaseTexture> texture0,
				   TexCoordXForm texxform0,
				   const Color &color0,
				   LayerBlendMode layer_blend_mode,
				   bool WriteAlpha,
				   const ROPConfig &ROP);


    void PushLayer(GraphicsEngine &graphics_engine, const Geometry &geo, AbstractPaintLayer *layer);
    void PushDrawLayer(GraphicsEngine &graphics_engine, const Geometry &geo, AbstractPaintLayer *layer);

    //! Render a paint layer.
    /*!
        When calling this function make sure to assign a correct geometry to the paint layer parameter.
    */
    void RenderSinglePaintLayer(GraphicsEngine &graphics_engine, Geometry geo, AbstractPaintLayer *paint_layer);

    //! Deprecated. Use PopPaintLayer.
    void PopBackground(int level = 1);

    //! Pop the top most paint layers from the active paint layer stack.
    /*!
        Pop the top most paint layers from the active paint layer stack.

        @param level The number of layer to pop off.
    */
    void PopPaintLayer(int level = 1);
    
    //! Deprecated. Use EmptyActivePaintLayerStack.
    void EmptyBackgroundStack();
    
    //! Empty the active paint layer stack.
    /*!
        Empty the active paint layer stack. All paint layers in the active stack are deleted.

    */
    void EmptyActivePaintLayerStack();

    //! Deprecated. Use PaintActivePaintLayerStack.
    void PaintBackground(GraphicsEngine &graphics_engine, const Geometry &geo);

    //! Paint all the layers in the active paint layer stack.
    /*!
        Paint all the layers in the active paint layer stack.

    */
    void PaintActivePaintLayerStack(GraphicsEngine &graphics_engine, const Geometry &geo);

    //! Paint all layers in all stacks
    /*
        Paint all layers in all stacks with the current model-view and projection matrices.
        This is useful for redirected rendering to textures.
    */
    void PaintAllLayerStack(GraphicsEngine& graphics_engine, const Geometry& geo);

    //! Deprecated. Use PushPaintLayerStack.
    void PushBackgroundStack();

    //! Push the current paint layer stack.
    /*!
        Push the current paint layer stack.
    */
    void PushPaintLayerStack();
    
    //! Deprecated. Use PopPaintLayerStack.
    void PopBackgroundStack();

    //! Pop a previously pushed paint layer stack.
    /*!
        Pop a previously pushed paint layer stack.
    */
    void PopPaintLayerStack();

  private:
    
    //! Clear all the pushed paint layers.
    /*!
        Clear all the pushed paint layers.
        Each layer in a paint layer stack is deleted.
    */
    void EmptyPushedPaintLayerStack();
    std::list<AbstractPaintLayer*> active_paint_layer_stack_;
    std::list<std::list<AbstractPaintLayer*> > pushed_paint_layer_stack_;

    WindowThread *window_thread_; //!< The WindowThread to which this object belongs.

  };


  class PushBackgroundScope
  {
  public:
    PushBackgroundScope(BasePainter &painter, GraphicsEngine &graphics_engine, const Geometry &geo, UXStyleImageRef image_style, bool PushAndDraw = false)
      : m_painter(painter)
    {
      if (PushAndDraw)
        m_painter.PushDrawShapeLayer(graphics_engine, geo, image_style, color::White, eAllCorners);
      else
        m_painter.PushShapeLayer(graphics_engine, geo, image_style, color::White, eAllCorners);
    }

    ~PushBackgroundScope()
    {
      //m_painter.PopBackground();
    }

  private:
    BasePainter &m_painter;
  };

  class PushShapeBackgroundScope
  {
  public:
    PushShapeBackgroundScope(BasePainter &painter, GraphicsEngine &graphics_engine, const Geometry &geo, UXStyleImageRef image_style, const Color &color, bool PushAndDraw = false, bool WriteAlpha = false, const ROPConfig &ROP = ROPConfig::Default)
      : m_painter(painter)
    {
      if (PushAndDraw)
        m_painter.PushDrawShapeLayer(graphics_engine, geo, image_style, color, eAllCorners, WriteAlpha, ROP);
      else
        m_painter.PushShapeLayer(graphics_engine, geo, image_style, color, eAllCorners, WriteAlpha, ROP);
    }
    ~PushShapeBackgroundScope()
    {
      m_painter.PopBackground();
    }
  private:
    BasePainter &m_painter;
  };

  class PushShapeCornerBackgroundScope
  {
  public:
    PushShapeCornerBackgroundScope(BasePainter &painter, GraphicsEngine &graphics_engine, const Geometry &geo, UXStyleImageRef image_style, const Color &color, long corners, bool PushAndDraw = false, bool WriteAlpha = false, const ROPConfig &ROP = ROPConfig::Default)
      : m_painter(painter)
    {
      if (PushAndDraw)
        m_painter.PushDrawShapeLayer(graphics_engine, geo, image_style, color, corners, WriteAlpha, ROP);
      else
        m_painter.PushShapeLayer(graphics_engine, geo, image_style, color, corners, WriteAlpha, ROP);
    }
    ~PushShapeCornerBackgroundScope()
    {
      m_painter.PopBackground();
    }
  private:
    BasePainter &m_painter;
  };

  class PushColorBackgroundScope
  {
  public:
    PushColorBackgroundScope(BasePainter &painter, GraphicsEngine &graphics_engine, const Geometry &geo, const Color &color, bool PushAndDraw = false, bool WriteAlpha = false, const ROPConfig &ROP = ROPConfig::Default)
      : m_painter(painter)
    {
      if (PushAndDraw)
        m_painter.PushDrawColorLayer(graphics_engine, geo, color, WriteAlpha, ROP);
      else
        m_painter.PushColorLayer(graphics_engine, geo, color, WriteAlpha, ROP);
    }

    ~PushColorBackgroundScope()
    {
      m_painter.PopBackground();
    }
  private:
    BasePainter &m_painter;
  };


}

#endif // PAINTER_H
