/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/TextEntry.h"
#include "Nux/ScrollView.h"
#include "Nux/GridHLayout.h"
#include "Nux/GridVLayout.h"
#include "Nux/TextureArea.h"
#include "Nux/Panel.h"
#include "Nux/MenuPage.h"
#include "NuxCore/Logger.h"

#include "NuxGraphics/IOpenGLFrameBufferObject.h"
#include "NuxGraphics/IOpenGLBaseTexture.h"

nux::NString tile_vertex_shader = TEXT ("             \n\
  #version 110                                        \n\
  attribute vec4 position;                            \n\
  attribute vec4 texcoord;                            \n\
  attribute vec4 color;                               \n\
  uniform mat4 mvp_matrix;                            \n\
  varying vec4 texcoord0;                             \n\
  varying vec4 color0;                                \n\
  void main()                                         \n\
  {                                                   \n\
    texcoord0 = texcoord;                             \n\
    color0 = color;                                   \n\
    gl_Position = mvp_matrix * position;              \n\
  }");

nux::NString tile_fragment_shader = TEXT ("           \n\
  #version 110                                        \n\
  varying vec4 texcoord0;                             \n\
  varying vec4 color0;                                \n\
  uniform sampler2D texture_unit0;                    \n\
  void main()                                         \n\
  {                                                   \n\
    gl_FragColor = color0;                            \n\
  }");


class Tile;

class TilesView: public nux::View
{
  NUX_DECLARE_OBJECT_TYPE(TilesView, View);
public:
  TilesView(NUX_FILE_LINE_PROTO);
  ~TilesView();

  void AddTile(Tile *tile);

protected:
  nux::Area* FindAreaUnderMouse(const nux::Point& mouse_position, nux::NuxEventType event_type);
  void OnMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
  void ShowPopupMenu(int x, int y);

  void Draw(nux::GraphicsEngine &graphics_engine, bool force_draw);
  void DrawContent(nux::GraphicsEngine &graphics_engine, bool force_draw);

private:
  Tile *focus_tile_;
  nux::GridHLayout *grid_layout_;
  nux::MenuPage *popup_menu_;
  nux::ColorLayer background_color_layer_;
};

class Tile: public nux::TextureArea
{
  NUX_DECLARE_OBJECT_TYPE(Tile, TextureArea);
public:
  Tile(NUX_FILE_LINE_PROTO);
  ~Tile();

protected:
  void OnMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void Draw(nux::GraphicsEngine &graphics_engine, bool force_draw);

private:
  nux::ObjectPtr<nux::IOpenGLFrameBufferObject> fbo_;
  nux::ObjectPtr<nux::IOpenGLBaseTexture> texture_;
  nux::ObjectPtr<nux::IOpenGLBaseTexture> depth_texture_;

  nux::ObjectPtr<nux::IOpenGLPixelShader>  fragment_shader_prog_;
  nux::ObjectPtr<nux::IOpenGLVertexShader> vertex_shader_prog_;
  nux::ObjectPtr<nux::IOpenGLShaderProgram> shader_prog_;
};

NUX_IMPLEMENT_OBJECT_TYPE(TilesView);

TilesView::TilesView(NUX_FILE_LINE_DECL)
: View(NUX_FILE_LINE_PARAM)
, background_color_layer_(nux::color::Orange, true)
{
  popup_menu_ = new nux::MenuPage("", NUX_TRACKER_LOCATION);
  popup_menu_->AddAction("Position");
  popup_menu_->AddAction("Remove");
  popup_menu_->AddAction("Configure");
  popup_menu_->SetParentObject(this);
  popup_menu_->SetOnClosureContinueEventCycle(true);

  background_color_layer_.SetColor(nux::color::Orange);
  
  mouse_up.connect(sigc::mem_fun(this, &TilesView::OnMouseDown));

  grid_layout_ = new nux::GridHLayout(NUX_TRACKER_LOCATION);
  grid_layout_->ForceChildrenSize(true);
  grid_layout_->SetChildrenSize(64, 64);
  grid_layout_->EnablePartialVisibility(false);

  grid_layout_->SetVerticalExternalMargin(32);
  grid_layout_->SetHorizontalExternalMargin(32);
  grid_layout_->SetVerticalInternalMargin(32);
  grid_layout_->SetHorizontalInternalMargin(32);

  grid_layout_->SetStretchFactor(1);
  grid_layout_->SetHeightMatchContent(false);

  SetLayout(grid_layout_);
}

TilesView::~TilesView()
{
  popup_menu_->UnParentObject();
}

void TilesView::AddTile(Tile *tile)
{
  if (tile == NULL)
    return;

  grid_layout_->AddView(tile, 1, nux::eLeft, nux::eFull);
}

nux::Area* TilesView::FindAreaUnderMouse(const nux::Point& mouse_position, nux::NuxEventType event_type)
{
  bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

  if(mouse_inside == false)
    return NULL;

  if (event_type == nux::NUX_MOUSE_PRESSED)
  {
    focus_tile_ = static_cast<Tile*>(grid_layout_->FindAreaUnderMouse(mouse_position, event_type));
  }
  else
  {
    //focus_tile_ = NULL;
  }
  
  if((event_type == nux::NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
    return NULL;
  return this;
}

void TilesView::OnMouseDown(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
{
  if (nux::GetEventButton(mouse_button_state) == nux::NUX_MOUSE_BUTTON3)
  {
    if (focus_tile_)
    {
      ShowPopupMenu(x + 2, y + 2);
    }
  }
}

void TilesView::ShowPopupMenu(int x, int y)
{
  popup_menu_->StopMenu();
  popup_menu_->StartMenu(x, y, 0, 0, false);
}

void TilesView::Draw(nux::GraphicsEngine &graphics_engine, bool force_draw)
{
  background_color_layer_.SetGeometry(GetGeometry());
  background_color_layer_.Renderlayer(graphics_engine);
}

void TilesView::DrawContent(nux::GraphicsEngine &graphics_engine, bool force_draw)
{
  nux::GetWindowThread()->GetPainter().PushLayer(graphics_engine, GetGeometry(), &background_color_layer_);

  if (m_CompositionLayout)
    m_CompositionLayout->ProcessDraw(graphics_engine, force_draw);

  nux::GetWindowThread()->GetPainter().PopBackground();
}

class TileManager
{
public:
  TileManager();
  ~TileManager();

  Tile* CreateTile();

  TilesView* GetTilesView();
private:
  TilesView* tiles_view_;
  nux::ObjectPtr<nux::IOpenGLFrameBufferObject> fbo_;
  nux::ObjectPtr<nux::IOpenGLBaseTexture> texture_;
  nux::ObjectPtr<nux::IOpenGLBaseTexture> depth_texture_;

};

TileManager::TileManager()
{
  tiles_view_ = new TilesView(NUX_TRACKER_LOCATION);
}

TileManager::~TileManager()
{

}

Tile* TileManager::CreateTile()
{
  Tile *tile = new Tile(NUX_TRACKER_LOCATION);
  tiles_view_->AddTile(tile);

  return tile;
}

TilesView* TileManager::GetTilesView()
{
  return tiles_view_;
}

NUX_IMPLEMENT_OBJECT_TYPE(Tile);

Tile::Tile(NUX_FILE_LINE_DECL)
: nux::TextureArea(NUX_FILE_LINE_PARAM)
{
  fbo_            = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateFrameBufferObject();
  texture_        = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(1, 1, 1, nux::BITFMT_R8G8B8A8);
  depth_texture_  = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(1, 1, 1, nux::BITFMT_D24S8);

  fragment_shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreatePixelShader();
  vertex_shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateVertexShader();
  shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateShaderProgram();

  vertex_shader_prog_->SetShaderCode(TCHAR_TO_ANSI(*tile_vertex_shader));
  fragment_shader_prog_->SetShaderCode(TCHAR_TO_ANSI(*tile_fragment_shader));

  shader_prog_->ClearShaderObjects();
  shader_prog_->AddShaderObject(vertex_shader_prog_);
  shader_prog_->AddShaderObject(fragment_shader_prog_);
  shader_prog_->Link();

}

Tile::~Tile()
{

}

void Tile::Draw(nux::GraphicsEngine &graphics_engine, bool force_draw)
{
  int width = GetBaseWidth();
  int height = GetBaseHeight();

  if ((texture_->GetWidth() != width) || (texture_->GetHeight() != height))
  {
    texture_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(width, height, 1, nux::BITFMT_R8G8B8A8);
  }
  
  if ((depth_texture_->GetWidth() != width) || (depth_texture_->GetHeight() != height))
  {
    depth_texture_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(width, height, 1, nux::BITFMT_D24S8);
  }

  //nux::ObjectPtr<nux::IOpenGLFrameBufferObject> prev_fbo = nux::GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
  fbo_->FormatFrameBufferObject(width, height, nux::BITFMT_R8G8B8A8);
  fbo_->SetRenderTarget(0, texture_->GetSurfaceLevel(0));
  fbo_->SetDepthSurface(depth_texture_->GetSurfaceLevel(0));
  fbo_->Activate();
  fbo_->EmptyClippingRegion();
  nux::GetGraphicsDisplay()->GetGraphicsEngine()->SetContext(0, 0, width, height);
  nux::GetGraphicsDisplay()->GetGraphicsEngine()->SetViewport(0, 0, width, height);
  nux::GetGraphicsDisplay()->GetGraphicsEngine()->Push2DWindow(width, height);

  nux::Geometry geo = GetAbsoluteGeometry();

  {
    nux::Color color = nux::color::RandomColor();

    float w = width;
    float h = height;
    float vertex_buffer[] =
    {
      0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0, 0, color.red, color.green, color.blue, color.alpha,
      0.0f, h,    0.0f, 1.0f, 0.0f, 1.0f, 0, 0, color.red, color.green, color.blue, color.alpha,
      w,    h,    0.0f, 1.0f, 1.0f, 1.0f, 0, 0, color.red, color.green, color.blue, color.alpha,
      w,    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0, 0, color.red, color.green, color.blue, color.alpha,
    };

    CHECKGL(glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    shader_prog_->Begin();

    int texture_unit0_location          = shader_prog_->GetUniformLocationARB("texture_unit0");
    int vertex_attrib_location          = shader_prog_->GetAttributeLocation("position");
    int texture_coord_attrib_location   = shader_prog_->GetAttributeLocation("texture");
    int color_attrib_location           = shader_prog_->GetAttributeLocation("color");

    if (texture_unit0_location != -1)
    {
      graphics_engine.SetTexture(GL_TEXTURE0, 0);
      CHECKGL(glUniform1iARB(texture_unit0_location, 0));
    }

    int mvp_matrix_location = shader_prog_->GetUniformLocationARB ("mvp_matrix");
    nux::Matrix4 mvp_matrix      = graphics_engine.GetOpenGLModelViewProjectionMatrix();
    shader_prog_->SetUniformLocMatrix4fv((GLint) mvp_matrix_location, 1, false, (GLfloat *) &(mvp_matrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(vertex_attrib_location));
    CHECKGL(glVertexAttribPointerARB ( (GLuint) vertex_attrib_location, 4, GL_FLOAT, GL_FALSE, 48, vertex_buffer));

    if (texture_coord_attrib_location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (texture_coord_attrib_location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) texture_coord_attrib_location, 4, GL_FLOAT, GL_FALSE, 48, vertex_buffer + 4) );
    }

    if (color_attrib_location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (color_attrib_location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) color_attrib_location, 4, GL_FLOAT, GL_FALSE, 48, vertex_buffer + 8) );
    }

    CHECKGL ( glDrawArrays (GL_TRIANGLE_FAN, 0, 4) );

    CHECKGL ( glDisableVertexAttribArrayARB (vertex_attrib_location) );

    if (texture_coord_attrib_location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (texture_coord_attrib_location) );

    if (color_attrib_location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (color_attrib_location) );

    shader_prog_->End();
  }

  //nux::GetGraphicsDisplay()->GetGraphicsEngine()->QRP_Color(0, 0, geo.width, geo.height, nux::color::Aubergine);

  nux::GetWindowCompositor().RestoreRenderingSurface();

  nux::TexCoordXForm texxform;
  nux::GetGraphicsDisplay()->GetGraphicsEngine()->QRP_1Tex(geo.x, geo.y, geo.width, geo.height, texture_, texxform, nux::color::White);
  //nux::TextureArea::Draw(graphics_engine, force_draw);
}


void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
  nux::VLayout* MainVLayout = new nux::VLayout(NUX_TRACKER_LOCATION);

  TileManager *tile_manager = new TileManager();
  for (int i = 0; i < 10; i++)
  {
    tile_manager->CreateTile();
    tile_manager->CreateTile();
    tile_manager->CreateTile();
    tile_manager->CreateTile();
    tile_manager->CreateTile();
  }

  MainVLayout->AddView(tile_manager->GetTilesView(), 1, nux::eCenter, nux::MINOR_SIZE_FULL);
  MainVLayout->SetContentDistribution(nux::eStackCenter);
  
  nux::GetWindowThread ()->SetLayout(MainVLayout);
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);
}

int main(int argc, char **argv)
{
  nux::NuxInitialize(0);

  nux::logging::Logger logger("test.tile_view");
  logger.SetLogLevel(nux::logging::Debug);
  LOG_DEBUG(logger) << "\nTest\n";

  nux::WindowThread* wt = nux::CreateGUIThread(TEXT("ScrollView"), 400, 300, 0, &ThreadWidgetInit, 0);
  wt->Run(NULL);

  delete wt;
  return 0;
}
