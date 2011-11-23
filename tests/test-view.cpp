
#include "Nux/Nux.h"
#include "test-view.h"
#include "NuxGraphics/GLTextureResourceManager.h"

nux::NString tile_vertex_shader = "             \n\
                                        #version 110                                      \n\
                                        attribute vec4 position;                          \n\
                                        attribute vec4 texcoord;                          \n\
                                        attribute vec4 color;                             \n\
                                        uniform mat4 mvp_matrix;                          \n\
                                        varying vec4 texcoord0;                           \n\
                                        varying vec4 color0;                              \n\
                                        void main()                                       \n\
                                        {                                                 \n\
                                        texcoord0 = texcoord;                             \n\
                                        color0 = color;                                   \n\
                                        gl_Position = mvp_matrix * position;              \n\
                                        }";

nux::NString tile_fragment_shader = "           \n\
                                          #version 110                                    \n\
                                          varying vec4 texcoord0;                         \n\
                                          varying vec4 color0;                            \n\
                                          uniform sampler2D texture_unit0;                \n\
                                          void main()                                     \n\
                                          {                                               \n\
                                          gl_FragColor = color0;                          \n\
                                          }";

std::string texture_vertex_code = "attribute vec4 AVertex;           \n\
  attribute vec4 MyTextureCoord0;                         \n\
  attribute vec4 VertexColor;                             \n\
  uniform mat4 ViewProjectionMatrix;                      \n\
  varying vec4 varyTexCoord0;                             \n\
  varying vec4 varyVertexColor;                           \n\
  void main()                                             \n\
  {                                                       \n\
    gl_Position =  ViewProjectionMatrix * (AVertex);      \n\
    varyTexCoord0 = MyTextureCoord0;                      \n\
    varyVertexColor = VertexColor;                        \n\
  }";

std::string texture_fragment_code = "varying vec4 varyTexCoord0;           \n\
  varying vec4 varyVertexColor;                               \n\
  uniform sampler2D TextureObject0;                           \n\
  vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)      \n\
  {                                                           \n\
    return texture2D(TexObject, TexCoord.st);                 \n\
  }                                                           \n\
  void main()                                                 \n\
  {                                                           \n\
    vec4 v = SampleTexture(TextureObject0, varyTexCoord0);    \n\
    gl_FragColor = v*varyVertexColor;                         \n\
  }";

NUX_IMPLEMENT_OBJECT_TYPE(TestView);

TestView::TestView(NUX_FILE_LINE_DECL)
  : nux::View(NUX_FILE_LINE_PARAM)
{
  ResetEvents();

  normal_color_ = nux::color::Green;
  mouse_down_color_ = nux::color::Red;
  mouse_drag_color_ = nux::color::Yellow;
  mouse_in_color_ = nux::color::Blue;
  current_color_ = normal_color_;

  mouse_in_ = false;
  mouse_mouse_drag_ = false;
  mouse_mouse_down_ = false;

  mouse_down.connect(sigc::mem_fun(this, &TestView::OnMouseDown));
  mouse_up.connect(sigc::mem_fun(this, &TestView::OnMouseUp));
  mouse_drag.connect(sigc::mem_fun(this, &TestView::OnMouseDrag));
  mouse_click.connect(sigc::mem_fun(this, &TestView::OnMouseClick));
  mouse_double_click.connect(sigc::mem_fun(this, &TestView::OnMouseDoubleClick));
  mouse_move.connect(sigc::mem_fun(this, &TestView::OnMouseMove));
  mouse_enter.connect(sigc::mem_fun(this, &TestView::OnMouseEnter));
  mouse_leave.connect(sigc::mem_fun(this, &TestView::OnMouseLeave));

  fbo_            = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateFrameBufferObject();
  texture_        = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(1, 1, 1, nux::BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
  depth_texture_  = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(1, 1, 1, nux::BITFMT_D24S8, NUX_TRACKER_LOCATION);

  fragment_shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreatePixelShader();
  vertex_shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateVertexShader();
  color_shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateShaderProgram();

  vertex_shader_prog_->SetShaderCode(TCHAR_TO_ANSI(*tile_vertex_shader));
  fragment_shader_prog_->SetShaderCode(TCHAR_TO_ANSI(*tile_fragment_shader));

  color_shader_prog_->ClearShaderObjects();
  color_shader_prog_->AddShaderObject(vertex_shader_prog_);
  color_shader_prog_->AddShaderObject(fragment_shader_prog_);
  color_shader_prog_->Link();

  // The shaders have been loaded into the program. They can be discarded.
  vertex_shader_prog_.Release();
  fragment_shader_prog_.Release();


  fragment_shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreatePixelShader();
  vertex_shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateVertexShader();
  texture_shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateShaderProgram();

  vertex_shader_prog_->SetShaderCode(texture_vertex_code.c_str());
  fragment_shader_prog_->SetShaderCode(texture_fragment_code.c_str());

  texture_shader_prog_->ClearShaderObjects();
  texture_shader_prog_->AddShaderObject(vertex_shader_prog_);
  texture_shader_prog_->AddShaderObject(fragment_shader_prog_);
  texture_shader_prog_->Link();

  // The shaders have been loaded into the program. They can be discarded.
  vertex_shader_prog_.Release();
  fragment_shader_prog_.Release();

  // color_ = nux::color::Aubergine;
  // nux::NBitmapData *bitmap = nux::LoadImageFile("bamboo.png");
  // nux::BaseTexture *base_texture = nux::CreateTextureFromBitmapData(bitmap);
  // image_texture_ = base_texture->GetDeviceTexture();
  // delete bitmap;
  // base_texture->UnReference();

}

TestView::~TestView()
{
  texture_.Release();
  depth_texture_.Release();
  texture_shader_prog_.Release();
  color_shader_prog_.Release();
}

void TestView::ResetEvents()
{
  registered_mouse_down_          = false;
  registered_mouse_up_            = false;
  registered_mouse_drag_          = false;
  registered_mouse_enter_         = false;
  registered_mouse_leave_         = false;
  registered_mouse_click_         = false;
  registered_mouse_double_click_  = false;
  registered_mouse_move_          = false;
  registered_mouse_enter_         = false;
  registered_mouse_leave_         = false;
    
}

nux::Color TestView::GetColor() const
{
  return current_color_;
}

void TestView::Draw(nux::GraphicsEngine &graphics_engine, bool force_draw)
{
  DrawColor(graphics_engine, force_draw);
  //DrawTexture(graphics_engine, force_draw);
}

void TestView::DrawColor(nux::GraphicsEngine &graphics_engine, bool force_draw)
{
  int width = GetBaseWidth();
  int height = GetBaseHeight();

  if ((texture_->GetWidth() != width) || (texture_->GetHeight() != height))
  {
    texture_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(width, height, 1, nux::BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
  }

  if ((depth_texture_->GetWidth() != width) || (depth_texture_->GetHeight() != height))
  {
    depth_texture_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(width, height, 1, nux::BITFMT_D24S8, NUX_TRACKER_LOCATION);
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
    float fh = height;
    float fw = width;

    nux::Color c = current_color_;
    float vertex_buffer[] =
    {
      0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0, 0, c.red, c.green, c.blue, c.alpha,
      0.0f, fh,   0.0f, 1.0f, 0.0f, 1.0f, 0, 0, c.red, c.green, c.blue, c.alpha,
      fw,   fh,   0.0f, 1.0f, 1.0f, 1.0f, 0, 0, c.red, c.green, c.blue, c.alpha,
      fw,   0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0, 0, c.red, c.green, c.blue, c.alpha,
    };

    CHECKGL(glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    color_shader_prog_->Begin();

    int texture_unit0_location          = color_shader_prog_->GetUniformLocationARB("texture_unit0");
    int vertex_attrib_location          = color_shader_prog_->GetAttributeLocation("position");
    int texture_coord_attrib_location   = color_shader_prog_->GetAttributeLocation("texture");
    int color_attrib_location           = color_shader_prog_->GetAttributeLocation("color");

    if (texture_unit0_location != -1)
    {
      graphics_engine.SetTexture(GL_TEXTURE0, 0);
      CHECKGL(glUniform1iARB(texture_unit0_location, 0));
    }

    int mvp_matrix_location = color_shader_prog_->GetUniformLocationARB ("mvp_matrix");
    nux::Matrix4 mvp_matrix      = graphics_engine.GetOpenGLModelViewProjectionMatrix();
    color_shader_prog_->SetUniformLocMatrix4fv((GLint) mvp_matrix_location, 1, false, (GLfloat *) &(mvp_matrix.m));

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

    color_shader_prog_->End();
  }

  //nux::GetGraphicsDisplay()->GetGraphicsEngine()->QRP_Color(0, 0, geo.width, geo.height, nux::color::Aubergine);

  nux::GetWindowCompositor().RestoreRenderingSurface();

  nux::TexCoordXForm texxform;
  nux::GetGraphicsDisplay()->GetGraphicsEngine()->QRP_1Tex(geo.x, geo.y, geo.width, geo.height, texture_, texxform, nux::color::White);
  //nux::TextureArea::Draw(graphics_engine, force_draw);
}

void TestView::DrawTexture(nux::GraphicsEngine &graphics_engine, bool force_draw)
{
  int width = GetBaseWidth();
  int height = GetBaseHeight();

  if ((texture_->GetWidth() != width) || (texture_->GetHeight() != height))
  {
    texture_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(width, height, 1, nux::BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
  }

  if ((depth_texture_->GetWidth() != width) || (depth_texture_->GetHeight() != height))
  {
    depth_texture_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(width, height, 1, nux::BITFMT_D24S8, NUX_TRACKER_LOCATION);
  }

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
    nux::TexCoordXForm texxform;
    nux::Color color = nux::color::White;
    QRP_Compute_Texture_Coord(width, height, image_texture_, texxform);
    float fx = 0, fy = 0;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 0, color.red, color.green, color.blue, color.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 0, color.red, color.green, color.blue, color.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 0, color.red, color.green, color.blue, color.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 0, color.red, color.green, color.blue, color.alpha,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    texture_shader_prog_->Begin();

    int TextureObjectLocation = texture_shader_prog_->GetUniformLocationARB("TextureObject0");
    int VertexLocation = texture_shader_prog_->GetAttributeLocation("AVertex");
    int TextureCoord0Location = texture_shader_prog_->GetAttributeLocation("MyTextureCoord0");
    int VertexColorLocation = texture_shader_prog_->GetAttributeLocation("VertexColor");

    graphics_engine.SetTexture(GL_TEXTURE0, image_texture_);
    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    int     VPMatrixLocation = texture_shader_prog_->GetUniformLocationARB("ViewProjectionMatrix");
    nux::Matrix4 MVPMatrix = graphics_engine.GetOpenGLModelViewProjectionMatrix();
    texture_shader_prog_->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    texture_shader_prog_->End();
  }

  nux::GetWindowCompositor().RestoreRenderingSurface();

  nux::TexCoordXForm texxform;
  texxform.flip_v_coord = true;
  nux::GetGraphicsDisplay()->GetGraphicsEngine()->QRP_1Tex(geo.x, geo.y, geo.width, geo.height, texture_, texxform, nux::color::White);
}

void TestView::OnMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_down_ = true;
  current_color_ = mouse_down_color_;
  mouse_mouse_down_ = true;
  QueueDraw();
}

void TestView::OnMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_up_ = true;
  if (mouse_in_)
    current_color_ = mouse_in_color_;
  else
    current_color_ = normal_color_;

  mouse_mouse_down_ = false;
  mouse_mouse_drag_ = false;
  QueueDraw();
}

void TestView::OnMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_drag_ = true;
  current_color_ = mouse_drag_color_;
  mouse_mouse_drag_ = true;
  QueueDraw();
}

void TestView::OnMouseClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_click_ = true;
  QueueDraw();
}

void TestView::OnMouseDoubleClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_double_click_ = true;
  current_color_ = mouse_down_color_;
  mouse_mouse_down_ = true;
  QueueDraw();
}

void TestView::OnMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_move_ = true;
  QueueDraw();
}

void TestView::OnMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_enter_ = true;
  mouse_in_ = true;
  current_color_ = mouse_in_color_;
  QueueDraw();
}

void TestView::OnMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_leave_ = true;
  mouse_in_ = false;
  current_color_ = normal_color_;
  QueueDraw();
}
