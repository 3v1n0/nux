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


#ifndef PAINTER_H
#define PAINTER_H

#include <string>
#include <iostream>
#include <list>

#include "Utils.h"
#include "NuxGraphics/OpenGLEngine.h"
#include "PaintLayer.h"

NAMESPACE_BEGIN_GUI

class TextLineRenderer;
class NTexture;

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
Geometry ComputeGeometryPositioning(const Geometry& container_geo, const Geometry& content_geo, GeometryPositioning gctx); 

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

    ~TextureAlignmentStyle(){};

    enum TextureAlignment horz_alignment;
    enum TextureAlignment vert_alignment;
    int horizontal_margin;
    int vertical_margin;
};

class BasePainter
{
public:
    BasePainter();
    virtual ~BasePainter();
    int intTest;

    //! Draw unscaled texture at position (x, y). The size of the quad is the size of the texture.
    virtual void Draw2DTexture(GraphicsContext& GfxContext, NTexture*Texture, int x, int y) const;
    virtual void Draw2DTextureAligned(GraphicsContext& GfxContext, NTexture*Texture, const Geometry &g, TextureAlignmentStyle tex_align) const;
    
    ///////////////////

    ////////////////////
    // TEXT PAINTING  //
    ////////////////////
    virtual int PaintColorTextLineEdit(GraphicsContext& GfxContext, const Geometry &g, const NString& Str,
        Color TextColor,
        bool WriteAlphaChannel,
        Color SelectedTextColor,
        Color SelectedTextBackgroundColor,
        Color TextBlinkColor,
        Color CursorColor,
        bool ShowCursor, unsigned int CursorPosition,
        int offset = 0,
        int selection_start = 0, int selection_end = 0) const;

    virtual int PaintTextLineStatic(GraphicsContext& GfxContext, const NFontPtr& Font, const Geometry &g,
        const NString& Str,
        const Color& color = Color(0.0f, 0.0f, 0.0f, 1.0f),
        bool WriteAlphaChannel = true,
        TextAlignment alignment = eAlignTextLeft) const;
    
    void Paint2DQuadColor(GraphicsContext& GfxContext, const Geometry &g, Color c0) const;
    void Paint2DQuadColor(GraphicsContext& GfxContext, const Geometry &g, Color c0_top_left, Color c1_bottom_left, Color c2_bottom_right, Color c3_top_right) const;

    void Paint2DQuadColor(GraphicsContext& GfxContext, int x, int y, int width, int height, Color c0) const;
    void Paint2DQuadColor(GraphicsContext& GfxContext, int x, int y, int width, int height, Color c0_top_left, Color c1_bottom_left, Color c2_bottom_right, Color c3_top_right) const;

    //! Paint a 2D quad with a gradient color going from Top to Bottom.
    /*! Paint a 2D quad with a gradient color going from Top to Bottom.
        @param g Geometry of the quad.
        @param TopColor color at the top of the quad.
        @param TopColor color at the bottom of the quad.
    */
    void Paint2DQuadVGradient(GraphicsContext& GfxContext, const Geometry &g, Color TopColor, Color BottomColor) const;
    //! Paint a 2D quad with a gradient color going from Left to Right.
    /*! Paint a 2D quad with a gradient color going from Left to Right.
        @param g Geometry of the quad.
        @param LeftColor color at the top of the quad.
        @param RightColor color at the bottom of the quad.
    */
    void Paint2DQuadHGradient(GraphicsContext& GfxContext, const Geometry &g, Color LeftColor, Color RightColor) const;

    void Paint2DQuadWireframe(GraphicsContext& GfxContext, const Geometry &g, Color c0) const;
    void Paint2DQuadWireframe(GraphicsContext& GfxContext, const Geometry &g, Color c_top_left, Color c_bottom_left, Color c_bottom_right, Color c_top_right) const;

    void Paint2DQuadWireframe(GraphicsContext& GfxContext, int x, int y, int width, int height, Color c0) const;
    void Paint2DQuadWireframe(GraphicsContext& GfxContext, int x, int y, int width, int height, Color c_top_left, Color c_bottom_left, Color c_bottom_right, Color c_top_right) const;

    void Draw2DTriangleColor(GraphicsContext& GfxContext, int x0, int y0,
        int x1, int y1,
        int x2, int y2,
        Color c0);
    void Draw2DTriangleColor(GraphicsContext& GfxContext, int x0, int y0,
        int x1, int y1,
        int x2, int y2,
        Color c0, Color c1, Color c2);

    //////////////////////
    // DRAW LINES       //
    //////////////////////
    void Draw2DLine(GraphicsContext& GfxContext, int x0, int y0,
        int x1, int y1, Color c0) const;
    void Draw2DLine(GraphicsContext& GfxContext, int x0, int y0,
        int x1, int y1, Color c0, Color c1) const;

    ////////////////////
    // Themes         //
    ////////////////////
    void PaintShape(GraphicsContext& GfxContext, const Geometry& geo, Color c0, UXStyleImageRef style, bool WriteAlpha = true) const;
    void PaintShapeCorner(GraphicsContext& GfxContext, const Geometry& geo, Color c0, UXStyleImageRef style, long corners, bool WriteAlpha = true) const;
    void PaintShapeROP(GraphicsContext& GfxContext, const Geometry& geo, Color c0, UXStyleImageRef style, bool WriteAlpha = true,
        const ROPConfig& ROP = ROPConfig::Default) const;
    void PaintShapeCornerROP(GraphicsContext& GfxContext, const Geometry& geo, Color c0, UXStyleImageRef style, long corners, bool WriteAlpha = true,
        const ROPConfig& ROP = ROPConfig::Default) const;
    
    void PaintTextureShape(GraphicsContext& GfxContext, const Geometry& geo, UXStyleImageRef style) const;
    void PaintTextureShape(GraphicsContext& GfxContext, const Geometry& geo, NTexture*Texture,
        int border_left, int border_right, int border_top, int border_bottom, bool draw_borders_only) const;

    //! Draw Check Box.
    /*!
        Draw a Check box.
    */
    
    void PaintCheckBox(GraphicsContext& GfxContext, const Geometry& geo, const InteractState& interaction_state,
        Color check_mark_color = Color(0x0), Color check_box_color = Color(0x0));

    void PaintRadioButton(GraphicsContext& GfxContext, const Geometry& geo, const InteractState& interaction_state,
        Color check_mark_color = Color(0x0), Color check_box_color = Color(0x0));

    void PaintHorizontalGradientQuad(GraphicsContext& GfxContext, const Geometry& geo, int num_color, float* percentage_array, Color* color_array);

public:
    void PushColorLayer(GraphicsContext& GfxContext, const Geometry& geo,
        Color color,
        bool WriteAlpha = false,
        const ROPConfig& ROP = ROPConfig::Default);

    void PushShapeLayer(GraphicsContext& GfxContext, Geometry geo,
        UXStyleImageRef imageStyle,
        const Color& color,
        unsigned long Corners = eAllCorners,
        bool WriteAlpha = false,
        const ROPConfig& ROP = ROPConfig::Default);

    void PushSliceScaledTextureLayer(GraphicsContext& GfxContext, Geometry geo,
        UXStyleImageRef imageStyle,
        const Color& color,
        unsigned long Corners = eAllCorners,
        bool WriteAlpha = false,
        const ROPConfig& ROP = ROPConfig::Default);

    void PushTextureLayer(GraphicsContext& GfxContext, Geometry geo,
        TRefGL<IOpenGLBaseTexture> DeviceTexture,
        TexCoordXForm texxform,
        const Color& color,
        bool WriteAlpha = false,
        const ROPConfig& ROP = ROPConfig::Default);

    void PushDrawColorLayer(GraphicsContext& GfxContext, const Geometry& geo,
        Color color,
        bool WriteAlpha = false,
        const ROPConfig& ROP = ROPConfig::Default);

    void PushDrawShapeLayer(GraphicsContext& GfxContext, Geometry geo,
        UXStyleImageRef imageStyle,
        const Color& color,
        unsigned long Corners = eAllCorners,
        bool WriteAlpha = false,
        const ROPConfig& ROP = ROPConfig::Default);

    void PushDrawSliceScaledTextureLayer(GraphicsContext& GfxContext, Geometry geo,
        UXStyleImageRef imageStyle,
        const Color& color,
        unsigned long Corners = eAllCorners,
        bool WriteAlpha = false,
        const ROPConfig& ROP = ROPConfig::Default);

    void PushDrawTextureLayer(GraphicsContext& GfxContext, Geometry geo,
        TRefGL<IOpenGLBaseTexture> DeviceTexture,
        TexCoordXForm texxform,
        const Color& color,
        bool WriteAlpha = false,
        const ROPConfig& ROP = ROPConfig::Default);
    
    void PushLayer(GraphicsContext& GfxContext, const Geometry& geo, AbstractPaintLayer* layer);
    void PushDrawLayer(GraphicsContext& GfxContext, const Geometry& geo, AbstractPaintLayer* layer);

    void RenderSinglePaintLayer(GraphicsContext& GfxContext, Geometry geo, AbstractPaintLayer* background);
   
    //! Pop a number of levels off the painter background stack.
    /*!
        Pop a number of levels off the painter background stack.
    */
    void PopBackground(int level = 1);
    void EmptyBackgroundStack();
    void PaintBackground(GraphicsContext& GfxContext, const Geometry& geo);

    std::list<AbstractPaintLayer*> m_BackgroundStack;

private:

};


class PushBackgroundScope
{
public:
    PushBackgroundScope(BasePainter& painter, GraphicsContext& GfxContext, const Geometry& geo, UXStyleImageRef image_style, bool PushAndDraw = false)
    : m_painter(painter)
    {
        if(PushAndDraw)
            m_painter.PushDrawShapeLayer(GfxContext, geo, image_style, Color::White, eAllCorners);
        else
            m_painter.PushShapeLayer(GfxContext, geo, image_style, Color::White, eAllCorners);
    }

    ~PushBackgroundScope()
    {
        //m_painter.PopBackground();
    }

private:
    BasePainter& m_painter;
};

class PushShapeBackgroundScope
{
public:
    PushShapeBackgroundScope(BasePainter& painter, GraphicsContext& GfxContext, const Geometry& geo, UXStyleImageRef image_style, const Color& color, bool PushAndDraw = false, bool WriteAlpha = false, const ROPConfig& ROP = ROPConfig::Default)
        : m_painter(painter)
    {
        if(PushAndDraw)
            m_painter.PushDrawShapeLayer(GfxContext, geo, image_style, color, eAllCorners, WriteAlpha, ROP);
        else
            m_painter.PushShapeLayer(GfxContext, geo, image_style, color, eAllCorners, WriteAlpha, ROP);
    }
    ~PushShapeBackgroundScope()
    {
        m_painter.PopBackground();
    }
private:
    BasePainter& m_painter;
};

class PushShapeCornerBackgroundScope
{
public:
    PushShapeCornerBackgroundScope(BasePainter& painter, GraphicsContext& GfxContext, const Geometry& geo, UXStyleImageRef image_style, const Color& color, long corners, bool PushAndDraw = false, bool WriteAlpha = false, const ROPConfig& ROP = ROPConfig::Default)
        : m_painter(painter)
    {
        if(PushAndDraw)
            m_painter.PushDrawShapeLayer(GfxContext, geo, image_style, color, corners, WriteAlpha, ROP);
        else
            m_painter.PushShapeLayer(GfxContext, geo, image_style, color, corners, WriteAlpha, ROP);
    }
    ~PushShapeCornerBackgroundScope()
    {
        m_painter.PopBackground();
    }
private:
    BasePainter& m_painter;
};

class PushColorBackgroundScope
{
public:
    PushColorBackgroundScope(BasePainter& painter, GraphicsContext& GfxContext, const Geometry& geo, const Color& color, bool PushAndDraw = false, bool WriteAlpha = false, const ROPConfig& ROP = ROPConfig::Default)
        : m_painter(painter)
    {
        if(PushAndDraw)
            m_painter.PushDrawColorLayer(GfxContext, geo, color, WriteAlpha, ROP);
        else
            m_painter.PushColorLayer(GfxContext, geo, color, WriteAlpha, ROP);
    }

    ~PushColorBackgroundScope()
    {
        m_painter.PopBackground();
    }
private:
    BasePainter& m_painter;
};


NAMESPACE_END_GUI

#endif // PAINTER_H