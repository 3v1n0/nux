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


#include "Nux.h"
#include "InputArea.h"
#include "Painter.h"

#include "NuxGraphics/GLTextureResourceManager.h"

namespace nux
{

  const unsigned int  STENCIL_TEST_REF = 0xFF;
  const unsigned int  STENCIL_TEST_MASK = 0xFFFFFFFF;
  const unsigned int  STENCIL_CLEAR = 0x0;
  const float         ALPHA_TEST_REF = 0.0f;

  GeometryPositioning::GeometryPositioning()
  {
    m_stretch_horizontal = true;
    m_stretch_vertical = true;

    m_vertical_aligment = eVACenter;
    m_horizontal_aligment = eHACenter;

    m_horizontal_margin = 0;
    m_vertical_margin = 0;
  }

  GeometryPositioning::GeometryPositioning (
    HorizontalAlignment horizontal_aligment,
    VerticalAlignment vertical_aligment,
    bool stretch_horizontal,
    bool stretch_vertical,
    int horizontal_margin,
    int vertical_margin)
  {
    m_stretch_horizontal = stretch_horizontal;
    m_stretch_vertical = stretch_vertical;

    m_vertical_aligment = eVACenter;
    m_horizontal_aligment = eHACenter;

    m_horizontal_margin = horizontal_margin;
    m_vertical_margin = vertical_margin;
  }

  GeometryPositioning::~GeometryPositioning()
  {

  }

  Geometry ComputeGeometryPositioning (const Geometry &container_geo, const Geometry &content_geo, GeometryPositioning gctx)
  {
    int x_pos, y_pos;
    int w, h;

    if (gctx.m_stretch_horizontal)
    {
      w = container_geo.GetWidth() - 2 * gctx.m_horizontal_margin;
    }
    else
    {
      w = content_geo.GetWidth();
    }

    if (gctx.m_stretch_horizontal)
    {
      h = container_geo.GetHeight() - 2 * gctx.m_vertical_margin;
    }
    else
    {
      h = content_geo.GetHeight();
    }

    if (gctx.m_horizontal_aligment == eHACenter)
    {
      int offset = (container_geo.GetWidth() - w) / 2.0f;
      //offset = (offset < 0) ? 0 : offset;
      x_pos = container_geo.x + offset;
    }
    else if (gctx.m_horizontal_aligment == eHALeft)
    {
      x_pos = container_geo.x + gctx.m_horizontal_margin;

      if (x_pos > container_geo.x + container_geo.GetWidth() )
        x_pos = container_geo.x;
    }
    else if (gctx.m_horizontal_aligment == eHARight)
    {
      x_pos = container_geo.x + container_geo.GetWidth() - w - gctx.m_horizontal_margin;

      if (x_pos < container_geo.x)
        x_pos = container_geo.x;
    }
    else
    {
      x_pos = container_geo.x + gctx.m_horizontal_margin;
    }

    if (gctx.m_vertical_aligment == eVACenter)
    {
      int offset = (container_geo.GetHeight() - h) / 2.0f;
      //offset = (offset < 0) ? 0 : offset;
      y_pos = container_geo.y + offset;
    }
    else if (gctx.m_vertical_aligment == eVATop)
    {
      y_pos = container_geo.y + gctx.m_vertical_margin;

      if (y_pos > container_geo.y + container_geo.GetHeight() )
        y_pos = container_geo.y;
    }
    else if (gctx.m_vertical_aligment == eVABottom)
    {
      y_pos = container_geo.y + container_geo.GetHeight() - h - gctx.m_vertical_margin;

      if (y_pos < container_geo.y)
        y_pos = container_geo.y;
    }
    else
    {
      y_pos = container_geo.y + gctx.m_vertical_margin;
    }

    return Geometry (x_pos, y_pos, w, h);
  }

  InteractState::InteractState()
  {
    is_on = false;
    is_focus = false;
    is_prelight = false;
    is_disable = false;
  }

  InteractState::InteractState (bool on, bool focus, bool prelight, bool disable)
  {
    is_on = on;
    is_focus = focus;
    is_prelight = prelight;
    is_disable = disable;
  }

  InteractState::~InteractState()
  {

  }

  BasePainter::BasePainter()
  {
  }

  BasePainter::~BasePainter()
  {
    EmptyBackgroundStack();
  }

  int BasePainter::PaintColorTextLineEdit (GraphicsEngine &GfxContext, const Geometry &g, const NString &Str,
      Color TextColor,
      bool WriteAlphaChannel,
      Color SelectedTextColor,
      Color SelectedTextBackgroundColor,
      Color TextBlinkColor,
      Color CursorColor,
      bool ShowCursor, unsigned int CursorPosition,
      int offset,
      int selection_start, int selection_end) const
  {
    PageBBox page;
    page.xmin = g.x;
    page.xmax = g.x + g.GetWidth();
    page.ymin = g.y;
    page.ymax = g.y + g.GetHeight();
    page.x_margin = DEFAULT_TEXT_X_MARGIN;
    page.y_margin = DEFAULT_TEXT_Y_MARGIN;
    return GfxContext.RenderColorTextLineEdit (GetSysFont(), page, Str,
           TextColor,
           WriteAlphaChannel,
           SelectedTextColor,
           SelectedTextBackgroundColor,
           TextBlinkColor,
           CursorColor,
           ShowCursor, CursorPosition, offset, selection_start, selection_end);
  }

  int BasePainter::PaintTextLineStatic (GraphicsEngine &GfxContext,
                                        ObjectPtr<FontTexture> Font,
                                        const Geometry &g,
                                        const NString &Str,
                                        const Color &color,
                                        bool WriteAlphaChannel,
                                        TextAlignment alignment) const
  {
    PageBBox page;
    page.xmin = g.x;
    page.xmax = g.x + g.GetWidth();
    page.ymin = g.y;
    page.ymax = g.y + g.GetHeight();
    page.x_margin = DEFAULT_TEXT_X_MARGIN;
    page.y_margin = DEFAULT_TEXT_Y_MARGIN;
    return GfxContext.RenderColorTextLineStatic (Font, page, Str, color, WriteAlphaChannel, alignment);
  }

  void BasePainter::Draw2DTexture (GraphicsEngine &GfxContext, BaseTexture *texture, int x, int y) const
  {
    float tex_w, tex_h;
    tex_w = (float) texture->GetWidth();
    tex_h = (float) texture->GetHeight();

    TexCoordXForm texxform;
    GfxContext.QRP_1Tex (x, y, tex_w, tex_h, texture->GetDeviceTexture(), texxform, color::White);
  }

  void BasePainter::Draw2DTextureAligned (GraphicsEngine &GfxContext, BaseTexture *texture, const Geometry &g, TextureAlignmentStyle tex_align) const
  {
    int x_pos, y_pos;
    int tex_w, tex_h;
    tex_w = (float) texture->GetWidth();
    tex_h = (float) texture->GetHeight();

    if (tex_align.horz_alignment == eTACenter)
    {
      int offset = (g.GetWidth() - tex_w) / 2.0f;
      offset = (offset < 0) ? 0 : offset;
      x_pos = g.x + offset;
    }
    else if (tex_align.horz_alignment == eTALeft)
    {
      x_pos = g.x + tex_align.horizontal_margin;
    }
    else if (tex_align.horz_alignment == eTARight)
    {
      x_pos = g.x + g.GetWidth() - tex_w - tex_align.horizontal_margin;
    }
    else
    {
      x_pos = g.x + tex_align.horizontal_margin;
    }

    if (tex_align.vert_alignment == eTACenter)
    {
      int offset = (g.GetHeight() - tex_h) / 2.0f;
      offset = (offset < 0) ? 0 : offset;
      y_pos = g.y + offset;
    }
    else if (tex_align.vert_alignment == eTATop)
    {
      y_pos = g.y + tex_align.vertical_margin;
    }
    else if (tex_align.vert_alignment == eTABottom)
    {
      y_pos = g.y + g.GetHeight() - tex_h - tex_align.vertical_margin;
    }
    else
    {
      y_pos = g.y + tex_align.vertical_margin;
    }

    PaintTextureShape (GfxContext, Geometry (x_pos, y_pos, tex_w, tex_h), texture, 0, 0, 0, 0, 0);
  }

///////////////////
// Draw QUADS    //
///////////////////

  void BasePainter::Paint2DQuadColor (GraphicsEngine &GfxContext, const Geometry &g, Color c0) const
  {
    GfxContext.QRP_Color (g.x, g.y, g.GetWidth(), g.GetHeight(), c0);
  }

  void BasePainter::Paint2DQuadColor (GraphicsEngine &GfxContext, const Geometry &g, Color c0_top_left, Color c1_bottom_left, Color c2_bottom_right, Color c3_top_right) const
  {
    GfxContext.QRP_Color (g.x, g.y, g.GetWidth(), g.GetHeight(), c0_top_left, c1_bottom_left, c2_bottom_right, c3_top_right);
  }

  void BasePainter::Paint2DQuadColor (GraphicsEngine &GfxContext, int x, int y, int width, int height, Color c0) const
  {
    GfxContext.QRP_Color (x, y, width, height, c0);
  }


  void BasePainter::Paint2DQuadColor (GraphicsEngine &GfxContext, int x, int y, int width, int height, Color c0_top_left, Color c1_bottom_left, Color c2_bottom_right, Color c3_top_right) const
  {
    GfxContext.QRP_Color (x, y, width, height, c0_top_left, c1_bottom_left, c2_bottom_right, c3_top_right);
  }

  void BasePainter::Paint2DQuadVGradient (GraphicsEngine &GfxContext, const Geometry &g, Color TopColor, Color BottomColor) const
  {
    Paint2DQuadColor (GfxContext, g, TopColor, BottomColor, BottomColor, TopColor);
  }

  void BasePainter::Paint2DQuadHGradient (GraphicsEngine &GfxContext, const Geometry &g, Color LeftColor, Color RightColor) const
  {
    Paint2DQuadColor (GfxContext, g, LeftColor, LeftColor, RightColor, LeftColor);
  }


/////////////////////////////
// Draw QUADS WIREFRAME    //
/////////////////////////////
  void BasePainter::Paint2DQuadWireframe (GraphicsEngine &GfxContext, const Geometry &g, Color c0) const
  {
    GfxContext.QRP_QuadWireframe (g.x, g.y, g.GetWidth(), g.GetHeight(), c0, c0, c0, c0);
  }

// void BasePainter::Paint2DQuadWireframe(GraphicsEngine& GfxContext, const Geometry &g, Color c0_left, Color c1_right) const
// {
//     GfxContext.QRP_QuadWireframe(g.x, g.y, g.GetWidth(), g.GetHeight(), c0_left, c0_left, c1_right, c1_right);
// }

  void BasePainter::Paint2DQuadWireframe (GraphicsEngine &GfxContext, const Geometry &g, Color c_top_left, Color c_bottom_left, Color c_bottom_right, Color c_top_right) const
  {
    GfxContext.QRP_QuadWireframe (g.x, g.y, g.GetWidth(), g.GetHeight(), c_top_left, c_bottom_left, c_bottom_right, c_top_right);
  }

  void BasePainter::Paint2DQuadWireframe (GraphicsEngine &GfxContext, int x, int y, int width, int height, Color c0) const
  {
    GfxContext.QRP_QuadWireframe (x, y, width, height, c0, c0, c0, c0);
  }

// void BasePainter::Paint2DQuadWireframe(GraphicsEngine& GfxContext, int x, int y, int width, int height, Color c0_left, Color c1_right) const
// {
//     GfxContext.QRP_QuadWireframe(x, y, width, height, c0_left, c0_left, c1_right, c1_right);
// }

  void BasePainter::Paint2DQuadWireframe (GraphicsEngine &GfxContext, int x, int y, int width, int height, Color c_top_left, Color c_bottom_left, Color c_bottom_right, Color c_top_right) const
  {
    GfxContext.QRP_QuadWireframe (x, y, width, height, c_top_left, c_bottom_left, c_bottom_right, c_top_right);
  }

//////////////////////
// Draw TRIANGLES   //
//////////////////////
  void BasePainter::Draw2DTriangleColor (GraphicsEngine &GfxContext, int x0, int y0,
                                         int x1, int y1,
                                         int x2, int y2,
                                         Color c0)
  {
    GfxContext.QRP_Triangle (x0, y0, x1, y1, x2, y2, c0);
  }

  void BasePainter::Draw2DTriangleColor (GraphicsEngine &GfxContext, int x0, int y0,
                                         int x1, int y1,
                                         int x2, int y2,
                                         Color c0, Color c1, Color c2)
  {
    GfxContext.QRP_Triangle (x0, y0, x1, y1, x2, y2, c0, c1, c2);
  }

//////////////////////
// DRAW LINES       //
//////////////////////
  void BasePainter::Draw2DLine (GraphicsEngine &GfxContext, int x0, int y0,
                                int x1, int y1, Color c0) const
  {
    GfxContext.QRP_Line (x0, y0, x1, y1, c0);
  }
  void BasePainter::Draw2DLine (GraphicsEngine &GfxContext, int x0, int y0,
                                int x1, int y1, Color c0, Color c1) const
  {
    GfxContext.QRP_Line (x0, y0, x1, y1, c0, c1);
  }

////////////////////
// Themes         //
////////////////////


  void BasePainter::PaintShape (GraphicsEngine &GfxContext, const Geometry &geo, Color c0, UXStyleImageRef style, bool WriteAlpha) const
  {
    ROPConfig ROP;
    ROP.Blend = true;
    ROP.SrcBlend = GL_SRC_ALPHA;
    ROP.DstBlend = GL_ONE_MINUS_SRC_ALPHA;
    PaintShapeCornerROP (GfxContext, geo, c0, style, eCornerTopLeft | eCornerTopRight | eCornerBottomLeft | eCornerBottomRight, WriteAlpha, ROP);
  }

  void BasePainter::PaintShapeCorner (GraphicsEngine &GfxContext, const Geometry &geo, Color c0, UXStyleImageRef style, long corners, bool WriteAlpha) const
  {
    ROPConfig ROP;
    ROP.Blend = true;
    ROP.SrcBlend = GL_SRC_ALPHA;
    ROP.DstBlend = GL_ONE_MINUS_SRC_ALPHA;
    PaintShapeCornerROP (GfxContext, geo, c0, style, corners, WriteAlpha, ROP);
  }

  void BasePainter::PaintShapeROP (GraphicsEngine &GfxContext, const Geometry &geo, Color c0, UXStyleImageRef style, bool WriteAlpha, const ROPConfig &ROP) const
  {
    PaintShapeCornerROP (GfxContext, geo, c0, style, eCornerTopLeft | eCornerTopRight | eCornerBottomLeft | eCornerBottomRight, WriteAlpha, ROP);
  }

  void BasePainter::PaintShapeCornerROP (GraphicsEngine &GfxContext, const Geometry &geo, Color c0, UXStyleImageRef style, long corners, bool WriteAlpha, const ROPConfig &ROP) const
  {
    const PainterImage *pimage = GetTheme().GetImage (style);

    if (pimage == 0)
      return;

    BaseTexture *texture = pimage->texture;

    int border_left = pimage->border_left;
    int border_right = pimage->border_right;
    int border_top = pimage->border_top;
    int border_bottom = pimage->border_bottom;
    bool draw_borders_only = pimage->draw_borders_only;
    t_u32 current_alpha_blend;
    t_u32 current_src_blend_factor;
    t_u32 current_dest_blend_factor;
    t_u32 current_red_mask;
    t_u32 current_green_mask;
    t_u32 current_blue_mask;
    t_u32 current_alpha_mask;

    // Get the current color mask and blend states. They will be restored later.
    GfxContext.GetRenderStates ().GetColorMask (current_red_mask, current_green_mask, current_blue_mask, current_alpha_mask);
    GfxContext.GetRenderStates ().GetBlend (current_alpha_blend, current_src_blend_factor, current_dest_blend_factor);


    GfxContext.GetRenderStates().SetColorMask (GL_TRUE, GL_TRUE, GL_TRUE, WriteAlpha ? GL_TRUE : GL_FALSE);
    GfxContext.GetRenderStates().SetBlend (ROP.Blend, ROP.SrcBlend, ROP.DstBlend);

    int tex_w = texture->GetWidth();
    int tex_h = texture->GetHeight();

    int r_x = geo.x;
    int r_y = geo.y;
    int r_w = geo.GetWidth();
    int r_h = geo.GetHeight();

    TexCoordXForm texxform;
    texxform.SetTexCoordType (TexCoordXForm::UNNORMALIZED_COORD);

    if (r_w < border_left + border_right)
    {
      // Do not apply this correction: just show the drawing as it is;
      //border_left = border_right = 0;
    }

    if (r_h < border_top + border_bottom)
    {
      // Do not apply this correction: just show the drawing as it is;
      //border_top = border_bottom = 0;
    }

    // Draw TOP-LEFT CORNER
    if (corners & eCornerTopLeft)
    {
      texxform.u0 = 0;
      texxform.v0 = 0;
      texxform.u1 = border_left;
      texxform.v1 = border_top;
      GfxContext.QRP_ColorModTexAlpha (r_x, r_y, border_left, border_top, texture->GetDeviceTexture(), texxform, c0);
    }
    else
    {
      GfxContext.QRP_Color (r_x, r_y, border_left, border_top, c0);
    }

    // Draw TOP-RIGHT CORNER
    if (corners & eCornerTopRight)
    {
      texxform.u0 = tex_w - border_right;
      texxform.v0 = 0;
      texxform.u1 = tex_w;
      texxform.v1 = border_top;
      GfxContext.QRP_ColorModTexAlpha (r_x + r_w - border_right, r_y, border_right, border_top, texture->GetDeviceTexture(), texxform, c0);
    }
    else
    {
      GfxContext.QRP_Color (r_x + r_w - border_right, r_y, border_right, border_top, c0);
    }

    // Draw BOTTOM-LEFT CORNER
    if (corners & eCornerBottomLeft)
    {
      texxform.u0 = 0;
      texxform.v0 = tex_h - border_bottom;
      texxform.u1 = border_left;
      texxform.v1 = tex_h;
      GfxContext.QRP_ColorModTexAlpha (r_x, r_y + r_h - border_bottom, border_left, border_bottom, texture->GetDeviceTexture(), texxform, c0);
    }
    else
    {
      GfxContext.QRP_Color (r_x, r_y + r_h - border_bottom, border_left, border_bottom, c0);
    }

    // Draw BOTTOM-RIGHT CORNER
    if (corners & eCornerBottomRight)
    {
      texxform.u0 = tex_w - border_right;
      texxform.v0 = tex_h - border_bottom;
      texxform.u1 = tex_w;
      texxform.v1 = tex_h;
      GfxContext.QRP_ColorModTexAlpha (r_x + r_w - border_right, r_y + r_h - border_bottom, border_right, border_bottom, texture->GetDeviceTexture(), texxform, c0);
    }
    else
    {
      GfxContext.QRP_Color (r_x + r_w - border_right, r_y + r_h - border_bottom, border_right, border_bottom, c0);
    }

    texxform.u0 = border_left;
    texxform.v0 = 0;
    texxform.u1 = tex_w - border_right;
    texxform.v1 = border_top;
    GfxContext.QRP_ColorModTexAlpha (r_x + border_left, r_y, r_w - border_left - border_right, border_top, texture->GetDeviceTexture(), texxform, c0);
    // Draw BOTTOM BORDER
    texxform.u0 = border_left;
    texxform.v0 = tex_h - border_bottom;
    texxform.u1 = tex_w - border_right;
    texxform.v1 = tex_h;
    GfxContext.QRP_ColorModTexAlpha (r_x + border_left, r_y + r_h - border_bottom, r_w - border_left - border_right, border_bottom, texture->GetDeviceTexture(), texxform, c0);
    // Draw LEFT BORDER
    texxform.u0 = 0;
    texxform.v0 = border_top;
    texxform.u1 = border_left;
    texxform.v1 = tex_h - border_bottom;
    GfxContext.QRP_ColorModTexAlpha (r_x, r_y + border_top, border_left, r_h - border_top - border_bottom, texture->GetDeviceTexture(), texxform, c0);
    // Draw RIGHT BORDER
    texxform.u0 = tex_w - border_right;
    texxform.v0 = border_top;
    texxform.u1 = tex_w;
    texxform.v1 = tex_h - border_bottom;
    GfxContext.QRP_ColorModTexAlpha (r_x + r_w - border_right, r_y + border_top, border_right, r_h - border_top - border_bottom, texture->GetDeviceTexture(), texxform, c0);

    // Draw CENTER
    if (draw_borders_only == false)
    {
      texxform.u0 = border_left;
      texxform.v0 = border_top;
      texxform.u1 = tex_w - border_right;
      texxform.v1 = tex_h - border_bottom;
      GfxContext.QRP_ColorModTexAlpha (r_x + border_left, r_y + border_top, r_w - border_left - border_right, r_h - border_top - border_bottom, texture->GetDeviceTexture(), texxform, c0);
    }

    // Restore Color mask and blend states.
    GfxContext.GetRenderStates ().SetColorMask (current_red_mask, current_green_mask, current_blue_mask, current_alpha_mask);
    GfxContext.GetRenderStates ().SetBlend (current_alpha_blend, current_src_blend_factor, current_dest_blend_factor);
  }

  void BasePainter::PaintTextureShape (GraphicsEngine &GfxContext, const Geometry &geo, UXStyleImageRef style) const
  {
    const PainterImage *pimage = GetTheme().GetImage (style);

    if (pimage == 0)
      return;

    BaseTexture *texture = pimage->texture;


    int border_left = pimage->border_left;
    int border_right = pimage->border_right;
    int border_top = pimage->border_top;
    int border_bottom = pimage->border_bottom;
    bool draw_borders_only = pimage->draw_borders_only;

    PaintTextureShape (GfxContext, geo, texture,
                       border_left, border_right, border_top, border_bottom, draw_borders_only);
  }

  void BasePainter::PaintTextureShape (GraphicsEngine &GfxContext, const Geometry &geo, BaseTexture *texture,
                                       int border_left, int border_right, int border_top, int border_bottom,
                                       bool draw_borders_only) const
  {
    int tex_w = texture->GetWidth();
    int tex_h = texture->GetHeight();

    int r_x = geo.x;
    int r_y = geo.y;
    int r_w = geo.GetWidth();
    int r_h = geo.GetHeight();

    if (r_w < border_left + border_right)
    {
      border_left = border_right = 0;
    }

    if (r_h < border_top + border_bottom)
    {
      border_top = border_bottom = 0;
    }

    GfxContext.GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    TexCoordXForm texxform;
    texxform.SetTexCoordType (TexCoordXForm::UNNORMALIZED_COORD);

    // Draw TOP-LEFT CORNER
    texxform.u0 = 0;
    texxform.v0 = 0;
    texxform.u1 = border_left;
    texxform.v1 = border_top;
    GfxContext.QRP_1Tex (r_x, r_y, border_left, border_top, texture->GetDeviceTexture(), texxform, color::White);
    // Draw TOP-RIGHT CORNER
    texxform.u0 = tex_w - border_right;
    texxform.v0 = 0;
    texxform.u1 = tex_w;
    texxform.v1 = border_top;
    GfxContext.QRP_1Tex (r_x + r_w - border_right, r_y, border_right, border_top, texture->GetDeviceTexture(), texxform, color::White);
    // Draw BOTTOM-LEFT CORNER
    texxform.u0 = 0;
    texxform.v0 = tex_h - border_bottom;
    texxform.u1 = border_left;
    texxform.v1 = tex_h;
    GfxContext.QRP_1Tex (r_x, r_y + r_h - border_bottom, border_left, border_bottom, texture->GetDeviceTexture(), texxform, color::White);
    // Draw BOTTOM-RIGHT CORNER
    texxform.u0 = tex_w - border_right;
    texxform.v0 = tex_h - border_bottom;
    texxform.u1 = tex_w;
    texxform.v1 = tex_h;
    GfxContext.QRP_1Tex (r_x + r_w - border_right, r_y + r_h - border_bottom, border_right, border_bottom, texture->GetDeviceTexture(), texxform, color::White);
    // Draw TOP BORDER
    texxform.u0 = border_left;
    texxform.v0 = 0;
    texxform.u1 = tex_w - border_right;
    texxform.v1 = border_top;
    GfxContext.QRP_1Tex (r_x + border_left, r_y, r_w - border_left - border_right, border_top, texture->GetDeviceTexture(), texxform, color::White);
    // Draw BOTTOM BORDER
    texxform.u0 = border_left;
    texxform.v0 = tex_h - border_bottom;
    texxform.u1 = tex_w - border_right;
    texxform.v1 = tex_h;
    GfxContext.QRP_1Tex (r_x + border_left, r_y + r_h - border_bottom, r_w - border_left - border_right, border_bottom, texture->GetDeviceTexture(), texxform, color::White);
    // Draw LEFT BORDER
    texxform.u0 = 0;
    texxform.v0 = border_top;
    texxform.u1 = border_left;
    texxform.v1 = tex_h - border_bottom;
    GfxContext.QRP_1Tex (r_x, r_y + border_top, border_left, r_h - border_top - border_bottom, texture->GetDeviceTexture(), texxform, color::White);
    // Draw RIGHT BORDER
    texxform.u0 = tex_w - border_right;
    texxform.v0 = border_top;
    texxform.u1 = tex_w;
    texxform.v1 = tex_h - border_bottom;
    GfxContext.QRP_1Tex (r_x + r_w - border_right, r_y + border_top, border_right, r_h - border_top - border_bottom, texture->GetDeviceTexture(), texxform, color::White);

    // Draw CENTER
    if (draw_borders_only == false)
    {
      texxform.u0 = border_left;
      texxform.v0 = border_top;
      texxform.u1 = tex_w - border_right;
      texxform.v1 = tex_h - border_bottom;
      GfxContext.QRP_1Tex (r_x + border_left, r_y + border_top, r_w - border_left - border_right, r_h - border_top - border_bottom, texture->GetDeviceTexture(), texxform, color::White);
    }

    GfxContext.GetRenderStates().SetBlend (FALSE);
  }

  void BasePainter::PaintHorizontalGradientQuad (GraphicsEngine &GfxContext, const Geometry &geo, int array_size, float *percentage_array, Color *color_array)
  {
    for (int i = 0; i < array_size - 1; i++)
    {
      float p0 = percentage_array[i] / 100.0f;
      float p1 = percentage_array[i+1] / 100.0f;

      Paint2DQuadColor (GfxContext, geo.x, geo.y + geo.GetHeight() * p0, geo.GetWidth(), geo.GetHeight() * p1 - geo.GetHeight() * p0,
                        color_array[i], color_array[i],
                        color_array[i+1], color_array[i+1]);
    }
  }

  void BasePainter::PaintCheckBox (GraphicsEngine &GfxContext, const Geometry &geo, const InteractState &interaction_state,
                                   Color check_mark_color, Color check_box_color)
  {

    GeometryPositioning pctx (eHACenter, eVACenter);
    pctx.m_stretch_horizontal = false;
    pctx.m_stretch_vertical = false;

    const PainterImage *pimage = GetTheme().GetImage (eCHECKBOX_NORMAL_ON);

    if (pimage == 0)
      return;

    BaseTexture *texture = pimage->texture;
    Geometry content_geo (0, 0, texture->GetWidth(), texture->GetHeight() ) ;
    content_geo = ComputeGeometryPositioning (geo, content_geo, pctx);

    GfxContext.GetRenderStates().SetBlend (true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (interaction_state.is_focus && interaction_state.is_on)
    {
      Draw2DTexture (GfxContext, GetTheme().GetImage (eCHECKBOX_FOCUS_ON)->texture, content_geo.x, content_geo.y);
    }
    else if (interaction_state.is_focus && !interaction_state.is_on)
    {
      Draw2DTexture (GfxContext, GetTheme().GetImage (eCHECKBOX_FOCUS_OFF)->texture, content_geo.x, content_geo.y);
    }
    else if (interaction_state.is_prelight && interaction_state.is_on)
    {
      Draw2DTexture (GfxContext, GetTheme().GetImage (eCHECKBOX_PRELIGHT_ON)->texture, content_geo.x, content_geo.y);
    }
    else if (interaction_state.is_prelight && !interaction_state.is_on)
    {
      Draw2DTexture (GfxContext, GetTheme().GetImage (eCHECKBOX_PRELIGHT_OFF)->texture, content_geo.x, content_geo.y);
    }
    else if (interaction_state.is_on)
    {
      Draw2DTexture (GfxContext, GetTheme().GetImage (eCHECKBOX_NORMAL_ON)->texture, content_geo.x, content_geo.y);
    }
    else
    {
      Draw2DTexture (GfxContext, GetTheme().GetImage (eCHECKBOX_NORMAL_OFF)->texture, content_geo.x, content_geo.y);
    }

    GfxContext.GetRenderStates().SetBlend (false);
    //PaintShape(content_geo, check_box_color, eSHAPE_CHECK_BOX);
    //Draw2DTexture(GetTheme().GetImage(eSHAPE_CHECK_BOX)->texture, content_geo);

    //PaintShape(const Geometry& geo, Color c0, UXStyleImageRef style)
  }

  void BasePainter::PaintRadioButton (GraphicsEngine &GfxContext, const Geometry &geo, const InteractState &interaction_state,
                                      Color check_mark_color, Color check_box_color)
  {
    PaintBackground (GfxContext, geo);
    GeometryPositioning pctx (eHACenter, eVACenter);
    pctx.m_stretch_horizontal = false;
    pctx.m_stretch_vertical = false;

    const PainterImage *pimage = GetTheme().GetImage (eRADIO_NORMAL_ON);

    if (pimage == 0)
      return;

    BaseTexture *texture = pimage->texture;

//     CachedBaseTexture* glr = (CachedBaseTexture*)GetGraphicsDisplay()->GetGraphicsEngine()->ResourceCache.GetCachedResource(texture);
//     glr->m_Texture->SetFiltering(GL_LINEAR, GL_LINEAR);

    Geometry content_geo (0, 0, texture->GetWidth(), texture->GetHeight() ) ;
    content_geo = ComputeGeometryPositioning (geo, content_geo, pctx);

    GfxContext.GetRenderStates().SetBlend (true, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    if (interaction_state.is_focus && interaction_state.is_on)
    {
      Draw2DTexture (GfxContext, GetTheme().GetImage (eRADIO_FOCUS_ON)->texture, content_geo.x, content_geo.y);
    }
    else if (interaction_state.is_focus && !interaction_state.is_on)
    {
      Draw2DTexture (GfxContext, GetTheme().GetImage (eRADIO_FOCUS_OFF)->texture, content_geo.x, content_geo.y);
    }
    else if (interaction_state.is_prelight && interaction_state.is_on)
    {
      Draw2DTexture (GfxContext, GetTheme().GetImage (eRADIO_PRELIGHT_ON)->texture, content_geo.x, content_geo.y);
    }
    else if (interaction_state.is_prelight && !interaction_state.is_on)
    {
      Draw2DTexture (GfxContext, GetTheme().GetImage (eRADIO_PRELIGHT_OFF)->texture, content_geo.x, content_geo.y);
    }
    else if (interaction_state.is_on)
    {
      Draw2DTexture (GfxContext, GetTheme().GetImage (eRADIO_NORMAL_ON)->texture, content_geo.x, content_geo.y);
    }
    else
    {
      Draw2DTexture (GfxContext, GetTheme().GetImage (eRADIO_NORMAL_OFF)->texture, content_geo.x, content_geo.y);
    }

    GfxContext.GetRenderStates().SetBlend (false);
  }

  void BasePainter::PaintBackground (GraphicsEngine &GfxContext, const Geometry &geo)
  {
    if (m_BackgroundStack.empty())
    {
      return;
    }

    std::list<AbstractPaintLayer *>::const_reverse_iterator rev_it;

    bool first = true;

    for (rev_it = m_BackgroundStack.rbegin (); rev_it != m_BackgroundStack.rend (); rev_it++)
    {
      AbstractPaintLayer *layer = (*rev_it);
      Geometry layer_geo = layer->GetGeometry ();
      Geometry xform_geo = GfxContext.ModelViewXFormRect (geo);

      GfxContext.PushClippingRectangle (geo);
      GfxContext.SetModelViewMatrix (layer->GetModelViewMatrix ());

      if (first)
      {
        Paint2DQuadColor (GfxContext, layer_geo, Color (0x0));
        first = false;
      }

      RenderSinglePaintLayer (GfxContext, layer_geo, layer);

      // restore the model view matrix stack and the clipping rectangle stack.
      GfxContext.ApplyModelViewMatrix ();
      GfxContext.PopClippingRectangle ();
    }
  }

  void BasePainter::RenderSinglePaintLayer (GraphicsEngine &GfxContext, Geometry geo, AbstractPaintLayer *paint_layer)
  {
    paint_layer->Renderlayer (GfxContext);
  }

  void BasePainter::PushLayer (GraphicsEngine &GfxContext, const Geometry &geo, AbstractPaintLayer *layer)
  {
    AbstractPaintLayer *l = layer->Clone();
    l->SetModelViewMatrix (GetGraphicsEngine ().GetModelViewMatrix ());
    l->SetGeometry (geo);
    m_BackgroundStack.push_front (l);
  }

  void BasePainter::PushDrawLayer (GraphicsEngine &GfxContext, const Geometry &geo, AbstractPaintLayer *layer)
  {
    PushLayer (GfxContext, geo, layer);
    PaintBackground (GfxContext, geo);
  }

  void BasePainter::PushColorLayer (GraphicsEngine &GfxContext, const Geometry &geo,
                                    Color color,
                                    bool WriteAlpha,
                                    const ROPConfig &ROP)
  {
    ColorLayer *cl = new ColorLayer (color, WriteAlpha, ROP);
    cl->SetModelViewMatrix (GetGraphicsEngine ().GetModelViewMatrix ());
    cl->SetGeometry (geo);
    m_BackgroundStack.push_front (cl);
  }

  void BasePainter::PushDrawColorLayer (GraphicsEngine &GfxContext, const Geometry &geo,
                                        Color color,
                                        bool WriteAlpha,
                                        const ROPConfig &ROP)
  {
    PushColorLayer (GfxContext, geo, color, WriteAlpha, ROP);
    PaintBackground (GfxContext, geo);
  }

  void BasePainter::PushShapeLayer (GraphicsEngine &GfxContext, Geometry geo,
                                    UXStyleImageRef imageStyle,
                                    const Color &color,
                                    unsigned long Corners,
                                    bool WriteAlpha,
                                    const ROPConfig &ROP)
  {
    ShapeLayer *sl = new ShapeLayer (imageStyle, color, Corners, WriteAlpha, ROP);
    sl->SetModelViewMatrix (GetGraphicsEngine ().GetModelViewMatrix ());
    sl->SetGeometry (geo);
    m_BackgroundStack.push_front (sl);
  }

  void BasePainter::PushDrawShapeLayer (GraphicsEngine &GfxContext, Geometry geo,
                                        UXStyleImageRef imageStyle,
                                        const Color &color,
                                        unsigned long Corners,
                                        bool WriteAlpha,
                                        const ROPConfig &ROP)
  {
    PushShapeLayer (GfxContext, geo, imageStyle, color, Corners, WriteAlpha, ROP);
    PaintBackground (GfxContext, geo);
  }

  void BasePainter::PushSliceScaledTextureLayer (GraphicsEngine &GfxContext, Geometry geo,
      UXStyleImageRef imageStyle,
      const Color &color,
      unsigned long Corners,
      bool WriteAlpha,
      const ROPConfig &ROP)
  {
    SliceScaledTextureLayer *sl = new SliceScaledTextureLayer (imageStyle, color, Corners, WriteAlpha, ROP);
    sl->SetModelViewMatrix (GetGraphicsEngine ().GetModelViewMatrix ());
    sl->SetGeometry (geo);
    m_BackgroundStack.push_front (sl);
  }

  void BasePainter::PushDrawSliceScaledTextureLayer (GraphicsEngine &GfxContext, Geometry geo,
      UXStyleImageRef imageStyle,
      const Color &color,
      unsigned long Corners,
      bool WriteAlpha,
      const ROPConfig &ROP)
  {
    PushSliceScaledTextureLayer (GfxContext, geo, imageStyle, color, Corners, WriteAlpha, ROP);
    PaintBackground (GfxContext, geo);
  }

  void BasePainter::PushTextureLayer (GraphicsEngine &GfxContext, Geometry geo,
                                      ObjectPtr<IOpenGLBaseTexture> DeviceTexture,
                                      TexCoordXForm texxform,
                                      const Color &color,
                                      bool WriteAlpha,
                                      const ROPConfig &ROP)
  {
    TextureLayer *tl = new TextureLayer (DeviceTexture, texxform, color, WriteAlpha, ROP);
    tl->SetModelViewMatrix (GetGraphicsEngine ().GetModelViewMatrix ());
    tl->SetGeometry (geo);
    m_BackgroundStack.push_front (tl);
  }

  void BasePainter::PushDrawTextureLayer (GraphicsEngine &GfxContext, Geometry geo,
                                          ObjectPtr<IOpenGLBaseTexture> DeviceTexture,
                                          TexCoordXForm texxform,
                                          const Color &color,
                                          bool WriteAlpha,
                                          const ROPConfig &ROP)
  {
    PushTextureLayer (GfxContext, geo, DeviceTexture, texxform, color, WriteAlpha, ROP);
    PaintBackground (GfxContext, geo);
  }

  void BasePainter::PopBackground (int level)
  {
    nuxAssert (level >= 0);

    while ( (level >= 1) && (m_BackgroundStack.size() > 0 ) )
    {
      AbstractPaintLayer *paint_layer = (*m_BackgroundStack.begin() );
      delete paint_layer;
      m_BackgroundStack.pop_front();
      level--;
    }
  }

  void BasePainter::EmptyBackgroundStack()
  {
    for (auto layer : m_BackgroundStack)
    {
      delete layer;
    }
    m_BackgroundStack.clear();
  }

}
