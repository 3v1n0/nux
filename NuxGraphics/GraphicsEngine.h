/*
 * Copyright 2010-2012 Inalogic® Inc.
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


#ifndef OPENGLENGINE_H
#define OPENGLENGINE_H

#include "GLResource.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"
#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GLRenderStates.h"
#include "FontTexture.h"
#include "RenderingPipe.h"
#include "GLShader.h"

#if defined(NUX_OS_WINDOWS)
  #include "GraphicsDisplay.h"
  #include "GLWindowManager.h"
#elif defined(NUX_OS_LINUX)
  #include "GraphicsDisplay.h"
  #include "GLWindowManager.h"
#endif

#define NUX_MAX_GAUSSIAN_SIGMA 11
#define NUX_MIN_GAUSSIAN_SIGMA 1

#ifndef NUX_OPENGLES_20
  #define NUX_VERTEX_SHADER_HEADER "#version 110               \n"
#else
  #define NUX_VERTEX_SHADER_HEADER 
#endif

#ifndef NUX_OPENGLES_20
  #define NUX_FRAGMENT_SHADER_HEADER "#version 110               \n"
#else
  #define NUX_FRAGMENT_SHADER_HEADER "precision mediump float;   \n"
#endif

namespace nux
{
  class FontTexture;
  class FontRenderer;
  class FilePath;
  class BaseTexture;
  class TextureRectangle;

  class ROPConfig
  {
  public:
    ROPConfig();
    ~ROPConfig();

    bool Blend;
    unsigned int SrcBlend;
    unsigned int DstBlend;

    static ROPConfig Default;
  };

  class BlendOperator
  {
  public:
    BlendOperator();
    ~BlendOperator();

    void EnableBlending(bool enable);
    void SetPorterDuffOperator(PorterDuffOperator op);
    void SetCustomBlendOperator(unsigned int src_blend, unsigned int dst_blend);

    bool  _enable;
    unsigned int _src_blend;
    unsigned int _dst_blend;
  };

  typedef struct
  {
    int x;
    int y;
    int width;
    int height;

  } OpenGLContext;

  class FxStructure
  {
  public:
    FxStructure();
    ~FxStructure();
    ObjectPtr<IOpenGLBaseTexture> dst_texture;
    ObjectPtr<IOpenGLBaseTexture> src_texture;
    ObjectPtr<IOpenGLBaseTexture> temp_texture;
  };

  typedef enum
  {
    LAYER_BLEND_MODE_NORMAL,
    // Darken Modes
    LAYER_BLEND_MODE_DARKEN,
    LAYER_BLEND_MODE_MULTIPLY,
    LAYER_BLEND_MODE_COLOR_BURN,
    LAYER_BLEND_MODE_LINEAR_BURN,
    // Lighten Modes
    LAYER_BLEND_MODE_LIGHTEN,
    LAYER_BLEND_MODE_SCREEN,
    LAYER_BLEND_MODE_COLOR_DODGE,
    LAYER_BLEND_MODE_LINEAR_DODGE,
    // Constrast Modes
    LAYER_BLEND_MODE_OVERLAY,
    LAYER_BLEND_MODE_SOFT_LIGHT,
    LAYER_BLEND_MODE_HARD_LIGHT,
    LAYER_BLEND_MODE_VIVID_LIGHT,
    LAYER_BLEND_MODE_LINEAR_LIGHT,
    LAYER_BLEND_MODE_PIN_LIGHT,
    LAYER_BLEND_MODE_HARD_MIX,
    // Inversion Modes
    LAYER_BLEND_MODE_DIFFERENCE,
    LAYER_BLEND_MODE_EXCLUSION,
    // Cancellation Modes
    LAYER_BLEND_MODE_SUBTRACT,
    // Others
    LAYER_BLEND_MODE_AVERAGE,
    LAYER_BLEND_MODE_ADD,
    LAYER_BLEND_MODE_NEGATION,
    LAYER_BLEND_MODE_REFLECT,
    LAYER_BLEND_MODE_GLOW,
    LAYER_BLEND_MODE_PHOENIX,
    //LAYER_BLEND_MODE_OPACITY,
    LAYER_BLEND_MODE_LAST
  } LayerBlendMode;


  //! Rendering engine class
  /*!
      This is the object that renders the graphics primitives.
  */
  class GraphicsEngine
  {
  public:
    /*!
      @param GlWindow The graphics window for this rendering engine.
      @param create_rendering_data If true, then in GraphicsEngine, the system creates the OpenGL shaders and the font textures for the rendering.
    */
    GraphicsEngine(GraphicsDisplay& GlWindow, bool create_rendering_data = true);
    ~GraphicsEngine();

    void SetContext(int x, int y, int width, int height);
    void GetContextSize(int& w, int& h) const;
    int GetContextWidth() const;
    int GetContextHeight() const;
    int GetContextX() const;
    int GetContextY() const;

    void GetWindowSize(int& w, int& h) const;
    int GetWindowWidth() const;
    int GetWindowHeight() const;

    OpenGLContext m_CurrrentContext;

    // Load Textures
    BaseTexture* Load2DTextureFile(const char* filename);
    BaseTexture* Load2DRectangleTextureFile(const char* filename);
    BaseTexture* Load2DTextureFileGenerateAlpha(const char* filename, int red, int green, int blue);

    void SetTexture(int TextureUnit, BaseTexture* Texture);
    void SetTexture(int TextureUnit, ObjectPtr< IOpenGLBaseTexture > Texture);
    void EnableTextureMode(int TextureUnit, int TextureMode);
    void DisableTextureMode(int TextureUnit, int TextureMode);
    void DisableAllTextureMode(int TextureUnit);

    ///////////////////
    // DRAW TEXTURE  //
    ///////////////////

    // Neutral
    //! Render a textured quad.
    /*!
        Shader Output = ve4(tex.r, tex.g, tex.b, tex.a)
    */
    void QRP_1Tex(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> Tex0, TexCoordXForm& texxform, const Color& color0);
    //! Render a textured quad. 
    /*!
        Multiply the shader rgb output with the texture alpha.
        Shader Output = ve4(tex.r*tex.a, tex.g*tex.a, tex.b*tex.a, tex.a)
    */
    void QRP_1TexPremultiply(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> Tex0, TexCoordXForm& texxform, const Color& color0);
    void QRP_TexDesaturate(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> Tex0, TexCoordXForm& texxform, const Color& color0, float desaturation_factor);    
    void QRP_Pixelate(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm& texxform, const Color& c0, int pixel_size);
    void QRP_Color(int x, int y, int width, int height, const Color& c0);
    void QRP_Color(int x, int y, int width, int height, const Color& c0, const Color& c1, const Color& c2, const Color& c3);
    void QRP_ColorModTexAlpha(int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm& texxform0, const Color& color);

    void QRP_2Tex(int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm& texxform0, const Color& color0,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm& texxform1, const Color& color1);

    void QRP_2TexMod(int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm& texxform0, const Color& color0,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm& texxform1, const Color& color1);

    void QRP_4Tex(int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm& texxform0, const Color& color0,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm& texxform1, const Color& color1,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture2, TexCoordXForm& texxform2, const Color& color2,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture3, TexCoordXForm& texxform3, const Color& color3);

    void QRP_Triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color c0);
    void QRP_Triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color c0, Color c1, Color c2);
    void QRP_Line(int x0, int y0, int x1, int y1, Color c0);
    void QRP_Line(int x0, int y0, int x1, int y1, Color c0, Color c1);
    void QRP_QuadWireframe(int x0, int y0, int width, int height, Color c0, Color c1, Color c2, Color c3);


    ObjectPtr<IOpenGLBaseTexture> QRP_GetBlurTexture(
      int x, int y,
      int buffer_width, int buffer_height,
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0,
      float sigma = 1.0f, int num_pass = 1);

    ObjectPtr<IOpenGLBaseTexture> QRP_GetAlphaTexture(
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0);

    ObjectPtr<IOpenGLBaseTexture> QRP_GetColorMatrixTexture(
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0,
      Matrix4 color_matrix, Vector4 offset);

    ObjectPtr<IOpenGLBaseTexture> QRP_GetPower(
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, const Vector4& exponent);

    ObjectPtr<IOpenGLBaseTexture> QRP_GetLQBlur(
      int x, int y, int buffer_width, int buffer_height,
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0);

    ObjectPtr<IOpenGLBaseTexture> QRP_GetHQBlur(
      int x, int y, int buffer_width, int buffer_height,
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0, float sigma = 1.0f, int num_pass = 1);

    void QRP_DisturbedTexture(
      int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> distorsion_texture, TexCoordXForm& texxform0, const Color& c0,
      ObjectPtr<IOpenGLBaseTexture> src_device_texture, TexCoordXForm& texxform1, const Color& c1);

	  ObjectPtr<IOpenGLBaseTexture> QRP_GetPixelBlocks(
		  ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform0, const Color& c0, int pixel_size);

    void QRP_GetCopyTexture(
      int width, int height,
      ObjectPtr<IOpenGLBaseTexture>& dst_device_texture,
      ObjectPtr<IOpenGLBaseTexture>& src_device_texture,
      TexCoordXForm& texxform0, const Color& c0);

#ifndef NUX_OPENGLES_20
    // ASM
    void QRP_ASM_1Tex(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> Tex0, TexCoordXForm& texxform, const Color& color0);
    void QRP_ASM_1TexPremultiply(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> Tex0, TexCoordXForm& texxform, const Color& color0);
    void QRP_ASM_Pixelate(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm& texxform, const Color& c0, int pixel_size);

    void QRP_ASM_Color(int x, int y, int width, int height, const Color& c0);
    void QRP_ASM_Color(int x, int y, int width, int height, const Color& c0, const Color& c1, const Color& c2, const Color& c3);
    void QRP_ASM_ColorModTexAlpha(int x, int y, int width, int height,
                               ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm& texxform0, const Color& color);

    void QRP_ASM_2Tex(int x, int y, int width, int height,
                   ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm& texxform0, const Color& color0,
                   ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm& texxform1, const Color& color1);

    void QRP_ASM_2TexMod(int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm& texxform0, const Color& color0,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm& texxform1, const Color& color1);

    void QRP_ASM_4Tex(int x, int y, int width, int height,
                   ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm& texxform0, const Color& color0,
                   ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm& texxform1, const Color& color1,
                   ObjectPtr<IOpenGLBaseTexture> DeviceTexture2, TexCoordXForm& texxform2, const Color& color2,
                   ObjectPtr<IOpenGLBaseTexture> DeviceTexture3, TexCoordXForm& texxform3, const Color& color3);

    void QRP_ASM_Triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color c0);
    void QRP_ASM_Triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color c0, Color c1, Color c2);
    void QRP_ASM_Line(int x0, int y0, int x1, int y1, Color c0);
    void QRP_ASM_Line(int x0, int y0, int x1, int y1, Color c0, Color c1);
    void QRP_ASM_QuadWireframe(int x0, int y0, int width, int height, Color c0, Color c1, Color c2, Color c3);

    void QRP_ASM_Power  (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, Vector4 exponent);
    void QRP_ASM_AlphaReplicate  (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0);
    void QRP_ASM_HorizontalGauss(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, float sigma = 1.0f);
    void QRP_ASM_VerticalGauss   (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, float sigma = 1.0f);
    void QRP_ASM_ColorMatrix     (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, Matrix4 color_matrix, Vector4 offset);

    //! Blur texture
    /*!
        @param device_texture The texture that is to be blurred.
        @param texxform       Texture transformation of device_texture.
        @param x              Position of the source texture in result buffer.
        @param y              Position of the source texture in result buffer.
        @param buffer_width   Width of result texture.
        @param buffer_height  Height of result texture.
        @param sigma          
    */
    ObjectPtr<IOpenGLBaseTexture> QRP_ASM_GetBlurTexture(
      int x, int y, int buffer_width, int buffer_height,
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& color, float sigma = 1.0f, int num_pass = 1);

    void QRP_ASM_GetBlurTextureFx(
      int x, int y, int buffer_width, int buffer_height,
      FxStructure* fx_structure, TexCoordXForm& texxform, const Color& color, float sigma = 1.0f, int num_pass = 1);

    //! Replicate the alpha channel in all components of the texture.
    /*!
        @param device_texture Source texture.
        @param texxform Texture transformation parameter.
        @param color Modulation color.
        @return Texture with all components set to the alpha value of the source.
    */
    ObjectPtr<IOpenGLBaseTexture> QRP_ASM_GetAlphaTexture(
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& color);

    //! Color matrix filter.
    /*!
        @param device_texture Source texture.
        @param texxform Texture transformation parameter.
        @param c Modulation color.
        @param color_matrix 4x4 section of the color matrix filter.
        @param offset Last column of the color matrix filter.
        @return Texture resulting from the processing of the source through a color matrix.
    */
    ObjectPtr<IOpenGLBaseTexture> QRP_ASM_GetColorMatrixTexture(
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c, Matrix4 color_matrix, Vector4 offset);

    //! Texture components raised to a power.
    /*!
        Each component of the texture is raised to a power provided in the exponent parameter.
        @param device_texture Source texture.
        @param texxform Texture transformation parameter.
        @param c Modulation color.
        @param exponent Power values for each component.
        @return A texture where the component of the source texture have been raised to a power.
    */
    ObjectPtr<IOpenGLBaseTexture> QRP_ASM_GetPower(
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, const Vector4& exponent);

    ObjectPtr<IOpenGLBaseTexture> QRP_ASM_GetLQBlur(
      int x, int y, int buffer_width, int buffer_height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0);

    ObjectPtr<IOpenGLBaseTexture> QRP_ASM_GetHQBlur(
      int x, int y, int buffer_width, int buffer_height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0, float sigma = 1.0f, int num_pass = 1);

    void QRP_ASM_DisturbedTexture(
      int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> distorsion_texture, TexCoordXForm& texxform0, const Color& c0,
      ObjectPtr<IOpenGLBaseTexture> src_device_texture, TexCoordXForm& texxform1, const Color& c1);

    //! Pixel blocks.
    /*!
        @param device_texture Source texture.
        @param texxform Texture transformation parameter.
        @param color Modulation color.
        @param pixel_size Size of pixel blocks.
        @return A texture pixelated version of the source texture.
    */
    ObjectPtr<IOpenGLBaseTexture>  QRP_ASM_GetPixelBlocks(
		  ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform0, const Color& color, int pixel_size);

    void QRP_ASM_GetCopyTexture(
      int width, int height,
      ObjectPtr<IOpenGLBaseTexture>& dst_device_texture,
      ObjectPtr<IOpenGLBaseTexture>& src_device_texture,
      TexCoordXForm& texxform0, const Color& c0);
#endif
    // GLSL

    void QRP_GLSL_1Tex(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm& texxform, const Color& c0);
    void QRP_GLSL_1TexPremultiply(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm& texxform, const Color& c0);
    void QRP_GLSL_TexDesaturate(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> Tex0, TexCoordXForm& texxform, const Color& color0, float desaturation_factor);
    void QRP_GLSL_Pixelate(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm& texxform, const Color& c0, int pixel_size);

    void QRP_GLSL_Color(int x, int y, int width, int height, const Color& c0);
    void QRP_GLSL_Color(int x, int y, int width, int height, const Color& c0, const Color& c1, const Color& c2, const Color& c3);
    void QRP_GLSL_ColorModTexAlpha(int x, int y, int width, int height,
                                    ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm& texxform0, const Color& color);

    void QRP_GLSL_2Tex(int x, int y, int width, int height,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm& texxform0, const Color& color0,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm& texxform1, const Color& color1);

    void QRP_GLSL_2TexMod(int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm& texxform0, const Color& color0,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm& texxform1, const Color& color1);


    void QRP_GLSL_4Tex(int x, int y, int width, int height,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm& texxform0, const Color& color0,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm& texxform1, const Color& color1,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture2, TexCoordXForm& texxform2, const Color& color2,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture3, TexCoordXForm& texxform3, const Color& color3);

    void QRP_GLSL_Triangle      (int x0, int y0, int x1, int y1, int x2, int y2, Color c0);
    void QRP_GLSL_Triangle      (int x0, int y0, int x1, int y1, int x2, int y2, Color c0, Color c1, Color c2);
    void QRP_GLSL_Line          (int x0, int y0, int x1, int y1, Color c0);
    void QRP_GLSL_Line          (int x0, int y0, int x1, int y1, Color c0, Color c1);
    void QRP_GLSL_QuadWireframe(int x0, int y0, int width, int height, Color c0, Color c1, Color c2, Color c3);

    void QRP_GLSL_Power           (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, Vector4 exponent);
    void QRP_GLSL_AlphaReplicate  (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0);
    void QRP_GLSL_HorizontalGauss(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, float sigma = 1.0f);
    void QRP_GLSL_VerticalGauss   (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, float sigma = 1.0f);
    void QRP_GLSL_HorizontalHQGauss(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, float sigma = 1.0f);
    void QRP_GLSL_VerticalHQGauss   (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, float sigma = 1.0f);
    void QRP_GLSL_HorizontalLSGauss(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, float sigma = 1.0f);
    void QRP_GLSL_VerticalLSGauss   (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, float sigma = 1.0f);
    void QRP_GLSL_ColorMatrix     (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, Matrix4 color_matrix, Vector4 offset);

    /*!
        @param device_texture The texture that is to be blurred.
        @param texxform       Texture transformation of device_texture.
        @param x              Position of the source texture in result buffer.
        @param y              Position of the source texture in result buffer.
        @param buffer_width   Width of result texture.
        @param buffer_height  Height of result texture.
    */
    ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetBlurTexture(
      int x, int y,
      int buffer_width, int buffer_height,
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0,
      float sigma = 1.0f, int num_pass = 1);
    
    ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetLSBlurTexture(
      int x, int y,
      int buffer_width, int buffer_height,
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0,
      float sigma = 1.0f, int num_pass = 1);

    ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetAlphaTexture(
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0);

    ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetColorMatrixTexture(
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0,
      Matrix4 color_matrix, Vector4 offset);

    ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetPower(
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform, const Color& c0, const Vector4& exponent);

    ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetLQBlur(
      int x, int y,
      int buffer_width, int buffer_height,
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0);

    ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetHQBlur(
      int x, int y,
      int buffer_width, int buffer_height,
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform,
      const Color& c0, float sigma = 1.0f, int num_pass = 1);

    void QRP_GLSL_GetHQBlurFx(
      int x, int y,
      int buffer_width, int buffer_height,
      FxStructure* fx_structure, TexCoordXForm& texxform,
      const Color& c0, float sigma = 1.0f, int num_pass = 1);
    
   void QRP_GLSL_GetLSBlurFx(
    int x, int y,
    int buffer_width, int buffer_height,
    FxStructure *fx_structure, TexCoordXForm &texxform,
    const Color& c0, float sigma = 1.0f, int num_pass = 1);

    void QRP_GLSL_DisturbedTexture(
      int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> distorsion_texture, TexCoordXForm& texxform0, const Color& c0,
      ObjectPtr<IOpenGLBaseTexture> src_device_texture, TexCoordXForm& texxform1, const Color& c1);

    ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetDisturbedTexture(
      int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> distorsion_texture, TexCoordXForm& texxform0, const Color& c0,
      ObjectPtr<IOpenGLBaseTexture> src_device_texture, TexCoordXForm& texxform1, const Color& c1);

    void QRP_GLSL_GetDisturbedTextureFx(
      int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> distorsion_texture, TexCoordXForm& texxform0, const Color& c0,
      FxStructure* fx_structure, TexCoordXForm& texxform1, const Color& c1);

    //! Pixel blocks.
    /*!
        @param device_texture Source texture.
        @param texxform Texture transformation parameter.
        @param color Modulation color.
        @param pixel_size Size of pixel blocks.
        @return A texture pixelated version of the source texture.
    */
	  ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetPixelBlocks(
		  ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm& texxform0, const Color& c0, int pixel_size);

    void QRP_GLSL_GetCopyTexture(
      int width, int height,
      ObjectPtr<IOpenGLBaseTexture>& dst_device_texture,
      ObjectPtr<IOpenGLBaseTexture>& src_device_texture,
      TexCoordXForm& texxform0, const Color& c0);

    //////////////////////
    // DRAW CLIPPING    //
    //////////////////////

    mutable std::vector<Rect> ClippingRect;
    void PushClippingRectangle(Rect const& rect);
    void PopClippingRectangle();
    void EmptyClippingRegion();
    //! Set the clipping according to the clipping rectangle stack.
    void ApplyClippingRectangle();

    void SetGlobalClippingRectangle(Rect const& rect);
    void DisableGlobalClippingRectangle();

    //! Bypass the clipping rectangle stack and set a different clipping rectangle region.
    /*!
        You may restore the clipping rectangle stack with ApplyClippingRectangle.
    */
    void SetClippingRectangle(const Rect& rect);

    //! Bypass the clipping rectangle stack and set a different clipping rectangle region.
    void SetOpenGLClippingRectangle(int x, int y, unsigned int width, unsigned int height);

    Rect GetClippingRegion() const;
    int GetNumberOfClippingRegions() const;

    void AddClipOffset(int x, int y);  //!< Deprecated. Use PushClipOffset.
    void PushClipOffset(int x, int y);
    void PopClipOffset();

    void ClearAreaColorDepthStencil(int x, int y, int width, int height, Color clearcolor, float cleardepth, int clearstencil);
    void ClearAreaColor(int x, int y, int width, int height, Color clearcolor);
    void ClearAreaDepthStencil(int x, int y, int width, int height, float cleardepth, int clearstencil);

    void Set3DView(int w, int h);

    //! Push a screen aligned 2D matrix.
    void Push2DWindow(int w, int h);
    void Pop2DWindow();
    void Push2DModelViewMatrix(Matrix4 mat);
    Matrix4 Pop2DModelViewMatrix();
    void Clear2DModelViewMatrix();
    std::list<Matrix4> m_2DModelViewMatricesStack;

    void SetViewport(int x, int y, int w, int h);
    Rect GetViewportRect() const;
    int  GetViewportWidth() const;
    int  GetViewportHeight() const;
    int  GetViewportX() const;
    int  GetViewportY() const;

    void  GetViewportSize(int& viewport_width, int& viewport_height) const;

    void SetScissor(int x, int y, int w, int h);

    /*!
        When setting a matrix to translate the widgets, the scissor region is not translated accordingly.
        This function provides a mean to offset the scissor region. It is useful when translating a widget during and overlay drawing.
        See an example in the code of NUX-01.
        @param x Clipping area offset.
        @param y Clipping area offset.
    */
    void SetScissorOffset(int x, int y);

    Rect const& GetScissorRect() const;
    void EnableScissoring(bool b);


//     void LoadPainterImages();
//     const PainterImage* GetImage(UXStyleImageRef style);
//     Rect GetImageGeometry(UXStyleImageRef style);
//     std::list<PainterImage*> m_PainterImageList;

    int RenderColorText(ObjectPtr<FontTexture> Font, int x, int y, std::string const& Str,
                         const Color& TextColor,
                         bool WriteAlphaChannel,
                         int NumCharacter);

    int RenderColorTextLineStatic(ObjectPtr<FontTexture> Font, const PageBBox& pageSize,
                                  std::string const& Str,
                                   const Color& TextColor,
                                   bool WriteAlphaChannel,
                                   TextAlignment alignment);

    int RenderColorTextLineEdit(ObjectPtr<FontTexture> Font, const PageBBox& pageSize,
                                std::string const& Str,
                                 const Color& TextColor,
                                 bool WriteAlphaChannel,
                                 const Color& SelectedTextColor,
                                 const Color& SelectedTextBackgroundColor,
                                 const Color& TextBlinkColor,
                                 const Color& CursorColor,
                                 bool ShowCursor, unsigned int CursorPosition,
                                 int offset = 0, int selection_start = 0, int selection_end = 0);

    ObjectPtr <IOpenGLBaseTexture> CreateTextureFromBackBuffer(int x, int y, int width, int height);

    //Statistics
    void ResetStats();

    /*!
        Cache a resource if it has previously been cached. If the resource does not contain valid data
        then the returned value is not valid. Check that the returned hardware resource is valid by calling ObjectPtr<CachedResourceData>.IsValid().
        @param Resource The resource to cache.
        @return A hardware resource.
    */
    ObjectPtr<CachedResourceData> CacheResource(ResourceData* Resource);

    /*!
        Update a resource if it has previously been cached.
        @param Resource The resource to cache.
    */
    void UpdateResource(ResourceData* Resource);

    bool FlushCachedResourceData(ResourceData* Resource);
    bool IsResourceCached(ResourceData* Resource);
    NResourceCache ResourceCache;

    Matrix4 GetProjectionMatrix();
    //! Return the transpose version of GetProjectionMatrix();
    Matrix4 GetOpenGLProjectionMatrix();
    
    void SetProjectionMatrix(const Matrix4& matrix);
    
    //! Set orthographic projection matrix.
    /*!
        The default projection matrix used by nux.

        @param viewport_width Viewport width.
        @param viewport_height Viewport height.
    */
    void SetOrthographicProjectionMatrix(int viewport_width, int viewport_height);

    //! Set orthographic projection matrix.
    /*!
        The default projection matrix used by nux.

        @param left coordinate of viewport location.
        @param right coordinate of viewport location.
        @param top coordinate of viewport location.
        @param bottom coordinate of viewport location.
    */
    void SetOrthographicProjectionMatrix(int left, int right, int bottom, int top);

    //! Reset the projection matrix to identity.
    void ResetProjectionMatrix();

    Matrix4 GetModelViewMatrix();
    //! Return the transpose version of GetModelViewMatrix();
    Matrix4 GetOpenGLModelViewMatrix();

    Matrix4 GetModelViewProjectionMatrix();
    Matrix4 GetOpenGLModelViewProjectionMatrix();

    GpuRenderStates& GetRenderStates()
    {
      return *_graphics_display.m_DeviceFactory->gpu_render_states_;
    }
    void ResetRenderStates()
    {
      _graphics_display.m_DeviceFactory->gpu_render_states_->ResetStateChangeToDefault();
    }
    void VerifyRenderStates()
    {
      _graphics_display.m_DeviceFactory->gpu_render_states_->CheckStateChange();
    }

    ObjectPtr<FontTexture> GetFont();
    ObjectPtr<FontTexture> GetBoldFont();

    //! Return True is GraphicsEngine is using the GLSL shader code path.
    /*!
        @return True is the system is using the GLSL shader code path.
    */
    bool UsingGLSLCodePath();

    //! Return True is GraphicsEngine is using the ARB program code path.
    /*!
        @return True is the system is using the ARB program code path.
    */
    bool UsingARBProgramCodePath();

    //! Push a model view matrix on the stack.
    void PushModelViewMatrix(const Matrix4& matrix);

    //! Push an Identity model view matrix on the stack.
    void PushIdentityModelViewMatrix();

    //! Push a 2D Translation model view matrix.
    /*!
        This is used by Nux to harmonize quads and lines pixel rendering in OpenGL.
    */
    void Push2DTranslationModelViewMatrix(float tx, float ty, float tz);

    //! Pop a model view matrix off the stack.
    /*!
        Return True is a matrix was successfully popped. False if there was no matrix to pop.
    */
    bool PopModelViewMatrix();

    //! Reset the model view matrix to identity.
    void ResetModelViewMatrixStack();

    //! Bypass the model view matrix stack and set a custom matrix.
    /*!
        You may restore the view matrix stack by calling ApplyModelViewMatrix.
    */
    void SetModelViewMatrix(const Matrix4& matrix);

    //! Set the model view matrix according to the model view matrix stack.
    void ApplyModelViewMatrix();

    //! Transform a rectangle with the model view matrix.
    /*!
        This transformation is only good as long as the model view matrix only contains 2D translations.
        The output rectangle width and height are the same as the input rectangle.

        @param rect The rectangle to transform.
    */
    Rect ModelViewXFormRect(const Rect& rect);

    //! Return the depth of the model view matrix stack.
    /*!
        @return The depth of the model view matrix stack.
    */
    int ModelViewStackDepth();



    //! Push a projection matrix on the stack.
    void PushProjectionMatrix(const Matrix4& matrix);

    //! Pop a projection matrix off the stack.
    /*!
        Return True is a matrix was successfully popped. False if there was no matrix to pop.
    */
    bool PopProjectionMatrix();

    //! Return the depth of the projection matrix stack.
    /*!
        @return The depth of the projection matrix stack.
    */
    int ProjectionStackDepth();





    //! Push a raster operation configuration setting on the stack.
    void PushPorterDuffBlend(const PorterDuffOperator& porter_duff_op);

    //! Push a state that disables the blending.
    void PushDisableBlend();

    //! Push a custom blend state.
    /*!
        @param src_blend OpenGL source blending mode.
        @param dst_blend OpenGL destination blending mode.
    */
    void PushBlend(unsigned int src_blend, unsigned int dst_blend);

    //! Pop a raster operation configuration setting off the stack.
    /*!
        Return True is a matrix was successfully popped. False if there was no matrix to pop.
    */
    bool PopBlend();

    //! Return the depth of the raster operation stack.
    /*!
        @return The depth of the raster operation stack.
    */
    int BlendStackDepth();

    //! Blends a color layer over a texture.
    /*!
        Blends a color over a texture layer.

        @param bkg_device_texture Background texture.
        @param frg_color Foreground color.
    */
    void QRP_GLSL_ColorLayerOverTexture(int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> bkg_device_texture, TexCoordXForm& bkg_texxform, const Color& bkg_color,
      const Color& frg_color,
      LayerBlendMode layer_blend_mode);

    //! Blends a texture layer over a color.
    /*!
        Blends a texture over a color layer.

        @param bkg_color Background color.
        @param frg_device_texture Foreground texture.
    */
    void QRP_GLSL_TextureLayerOverColor(int x, int y, int width, int height,
      const Color& bkg_color,
      ObjectPtr<IOpenGLBaseTexture> frg_device_texture, TexCoordXForm& frg_texxform, const Color& frg_color,
      LayerBlendMode layer_blend_mode);

    //! Blends a texture layer over a texture.
    /*!
        Uses a layer blending operation to render two textures.

        @param bkg_device_texture Background texture layer.
        @param frg_device_texture Foreground texture layer.
    */
    void QRP_GLSL_TextureLayerOverTexture(int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> bkg_device_texture, TexCoordXForm& bkg_texxform, const Color& bkg_color,
      ObjectPtr<IOpenGLBaseTexture> frg_device_texture, TexCoordXForm& frg_texxform, const Color& frg_color,
      LayerBlendMode layer_blend_mode);

    //! Blends a color layer over a color.
    /*!
        Uses a layer blending operation to render two colors.
        @param bkg_color Background color layer.
        @param frg_color Foreground color layer.
    */
    void QRP_GLSL_ColorLayerOverColor(int x, int y, int width, int height,
      const Color& bkg_color,
      const Color& frg_color,
      LayerBlendMode layer_blend_mode);

  private:

    ObjectPtr<FontTexture> _normal_font;    //!< The normal font renderer
    ObjectPtr<FontTexture> _bold_font;      //!< The bold font renderer

    void InitShaders();

    //! Helper function to compute a Gaussian filter weights
    void GaussianWeights(float **weights, float sigma, unsigned int num_tap);
    
    int LinearSampleGaussianWeights(std::vector<float>& weights, std::vector<float>& offsets, 
                                                  float sigma);

    //! Helper function to set an fbo
    void SetFrameBufferHelper(
      ObjectPtr<IOpenGLFrameBufferObject>& fbo,
      ObjectPtr<IOpenGLBaseTexture>& colorbuffer,
      ObjectPtr<IOpenGLBaseTexture>& depthbuffer,
      int width, int height);

#ifndef NUX_OPENGLES_20
    void InitAsmColorShader();
    //! Render polygons with a single color.
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmColor;

    void InitAsmTextureShader();
    //! Render polygons with a texture modulated by a color.
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmTextureModColor;
    //! Same as m_AsmTextureModColor for rectangle textures.
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmTextureRectModColor;

    void InitAsmTexturePremultiplyShader();
    //! Render polygons with a texture modulated by a color.
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmTexturePremultiplyModColor;
    //! Same as m_AsmTextureModColor for rectangle textures.
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmTexturePremultiplyRectModColor;

    void InitAsmPixelateShader();
    //! Render a pixelated texture over a polygon.
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmPixelate;
    //! Render a pixelated texture rectangle over a polygon.
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmPixelateRect;

    void InitAsmColorModTexMaskAlpha();
    //! Render polygons with a color masked by the alpha provided sampling a texture.
    //! Requires: Enable GPU Alpha Blending
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmColorModTexMaskAlpha;
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmColorModTexRectMaskAlpha;

    void InitAsm2TextureAdd();
    //! Render polygons with 2 textures, each modulated by a color, and added together.
    ObjectPtr<IOpenGLAsmShaderProgram> m_Asm2TextureAdd;
    ObjectPtr<IOpenGLAsmShaderProgram> m_Asm2TextureRectAdd;

    void InitAsm2TextureMod();
    //! Render polygons with 2 textures, each modulated by a color, and then multiplied together.
    ObjectPtr<IOpenGLAsmShaderProgram> m_Asm2TextureMod;
    ObjectPtr<IOpenGLAsmShaderProgram> m_Asm2TextureRectMod;

    void InitAsm4TextureAdd();
    //! Render polygons with 4 textures, each modulated by a color, and added together.
    ObjectPtr<IOpenGLAsmShaderProgram> m_Asm4TextureAdd;
    ObjectPtr<IOpenGLAsmShaderProgram> m_Asm4TextureRectAdd;

    void InitAsmPower();
    //! Raise a texture component to a power.
    /*!
        result = color0 * (tex0)^(exponent) = (tex0.r^exponent.r, tex0.g^exponent.g, tex0.b^exponent.b, tex0.a^exponent.a);
    */
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_tex_component_exponentiation_prog;
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_texrect_component_exponentiation_prog;

    void InitAsmAlphaReplicate();
    //! Replicate the alpha component into r, g and b.
    /*!
        result = color0 * (tex0.a, tex0.a, tex0.a, tex0.a);
    */
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_tex_alpha_replicate_prog;
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_texrect_alpha_replicate_prog;

    void InitAsmSeparableGaussFilter();
    //! Gauss horizontal filter.
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_tex_separable_gauss_filter_prog;
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_texrect_separable_gauss_filter_prog;

//     void InitAsmSeparableGaussFilter2();
//     //! Gauss horizontal filter.
//     ObjectPtr<IOpenGLAsmShaderProgram> _asm_tex_separable_gauss_filter_prog2;
//     ObjectPtr<IOpenGLAsmShaderProgram> _asm_texrect_separable_gauss_filter_prog2;

    void InitAsmColorMatrixFilter();
    //! Color matrix filter.
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_tex_color_matrix_filter_prog;
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_texrect_color_matrix_filter_prog;

    void InitAsm2TextureDepRead();
    ObjectPtr<IOpenGLAsmShaderProgram> m_ASM2TextureDepRead;
    ObjectPtr<IOpenGLAsmShaderProgram> m_ASM2TextureRectDepRead;

    void InitAsmBlendModes();

    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmPSBNormal;
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmPSBLighten;
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmPSBDarken;
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmPSBMultiply;
#endif

    void InitSlColorShader();
    //! Render polygons with a single color.
    ObjectPtr<IOpenGLShaderProgram> m_SlColor;

    void InitSlTextureShader();
    //! Render polygons with a texture modulated by a color.
    ObjectPtr<IOpenGLShaderProgram> m_SlTextureModColor;

    void InitSlTexturePremultiplyShader();
    //! Render polygons with a premultiplied texture modulated by a color.
    ObjectPtr<IOpenGLShaderProgram> m_SlTexturePremultiplyModColor;

    void InitSlPixelateShader();
    //! Render a pixelated texture over a polygon.
    ObjectPtr<IOpenGLShaderProgram> m_SLPixelate;

    void InitSlColorModTexMaskAlpha();
    //! Render polygons with a color masked by the alpha provided sampling a texture.
    //! Requires: Enable GPU Alpha Blending
    ObjectPtr<IOpenGLShaderProgram> m_SlColorModTexMaskAlpha;
    ObjectPtr<IOpenGLShaderProgram> m_SlColorModTexRectMaskAlpha;

    void InitSl2TextureAdd();
    //! Render polygons with 2 textures, each modulated by a color, and added together.
    //! result = (tex0*color0)+(tex1*color1)
    ObjectPtr<IOpenGLShaderProgram> m_Sl2TextureAdd;

    void InitSl2TextureDepRead();
    //! Render a quad with 2 textures. One texture sample is use to offset the coordinates of the second texture read.
    //! result = (tex1(coord1.xy + tex0.xy)*color1)
    ObjectPtr<IOpenGLShaderProgram> m_Sl2TextureDepRead;


    void InitSl2TextureMod();
    //! Render polygons with 2 textures, each modulated by a color, and then multiplied together.
    //! result = (tex0*color0)*(tex1*color1)
    ObjectPtr<IOpenGLShaderProgram> m_Sl2TextureMod;

    void InitSl4TextureAdd();
    //! Render polygons with 4 textures, each modulated by a color, and added together.
    ObjectPtr<IOpenGLShaderProgram> m_Sl4TextureAdd;

    void InitSLPower();
    //! Raise a texture component to a power.
    /*!
        result = color0 * (tex0)^(exponent) = (tex0.r^exponent.r, tex0.g^exponent.g, tex0.b^exponent.b, tex0.a^exponent.a);
    */
    ObjectPtr<IOpenGLShaderProgram> _component_exponentiation_prog;

    void InitSLAlphaReplicate();
    //! Replicate the alpha component into r, g and b.
    /*!
        result = color0 * (tex0.a, tex0.a, tex0.a, tex0.a);
    */
    ObjectPtr<IOpenGLShaderProgram> _alpha_replicate_prog;

    void InitSLHorizontalGaussFilter();
    //! Gauss horizontal filter.
    ObjectPtr<IOpenGLShaderProgram> _horizontal_gauss_filter_prog;
    
    void InitSLVerticalGaussFilter();
    //! Gauss vertical filter.
    ObjectPtr<IOpenGLShaderProgram> _vertical_gauss_filter_prog;
    

    void InitSLHorizontalHQGaussFilter(int sigma);
    //! Gauss horizontal filter.
    ObjectPtr<IOpenGLShaderProgram> _horizontal_hq_gauss_filter_prog[NUX_MAX_GAUSSIAN_SIGMA];

    void InitSLVerticalHQGaussFilter(int sigma);
    //! Gauss vertical filter.
    ObjectPtr<IOpenGLShaderProgram> _vertical_hq_gauss_filter_prog[NUX_MAX_GAUSSIAN_SIGMA];
    
    void InitSLHorizontalLSGaussFilter(int k);
    //! Gauss horizontal filter.
    ObjectPtr<IOpenGLShaderProgram> _horizontal_ls_gauss_filter_prog[NUX_MAX_GAUSSIAN_SIGMA];

    void InitSLVerticalLSGaussFilter(int k);
    //! Gauss vertical filter.
    ObjectPtr<IOpenGLShaderProgram> _vertical_ls_gauss_filter_prog[NUX_MAX_GAUSSIAN_SIGMA];

    void InitSLColorMatrixFilter();
    //! Color matrix filter.
    ObjectPtr<IOpenGLShaderProgram> _color_matrix_filter_prog;

    void InitSLDesaturation();
    ObjectPtr<IOpenGLShaderProgram> desaturation_prog_;

    void InitSlBlendModes();

    void InitBlendModeNormal();
    void InitBlendModeLighten();
    void InitBlendModeDarken();

    ObjectPtr<IOpenGLShaderProgram> shader_layer_blend_normal_;
    ObjectPtr<IOpenGLShaderProgram> shader_layer_blend_lighten_;
    ObjectPtr<IOpenGLShaderProgram> shader_layer_blend_darken_;
    ObjectPtr<IOpenGLShaderProgram> shader_layer_blend_multiply_;

    // Blend modes
    ObjectPtr<IOpenGLShaderProgram> blend_tex_color_prog_[LAYER_BLEND_MODE_LAST];
    ObjectPtr<IOpenGLShaderProgram> blend_color_tex_prog_[LAYER_BLEND_MODE_LAST];
    ObjectPtr<IOpenGLShaderProgram> blend_tex_tex_prog_[LAYER_BLEND_MODE_LAST];
    ObjectPtr<IOpenGLShaderProgram> blend_color_color_prog_[LAYER_BLEND_MODE_LAST];
    
    std::string GetBlendModeBlendFunc(LayerBlendMode layer_blend_mode);
    std::string GetBlendModeString(LayerBlendMode layer_blend_mode);
    
    ObjectPtr<IOpenGLShaderProgram> GetColorBlendOverTexProgram(LayerBlendMode layer_blend_mode);
    ObjectPtr<IOpenGLShaderProgram> GetTextureLayerOverColorProgram(LayerBlendMode layer_blend_mode);
    ObjectPtr<IOpenGLShaderProgram> GetTextureLayerOverTextureProgram(LayerBlendMode layer_blend_mode);
    ObjectPtr<IOpenGLShaderProgram> GetColorLayerOverColorProgram(LayerBlendMode layer_blend_mode);



    //! Test the gpu features and set variables such as \e _use_glsl_shaders.
    void EvaluateGpuCaps();

    bool _use_glsl_shaders; //!< True if the system is using the glsl code path.

    ObjectPtr<IOpenGLFrameBufferObject> _offscreen_fbo;
    ObjectPtr<IOpenGLBaseTexture> _offscreen_color_rt0;
    ObjectPtr<IOpenGLBaseTexture> _offscreen_depth_rt0;
    ObjectPtr<IOpenGLBaseTexture> _offscreen_color_rt1;
    ObjectPtr<IOpenGLBaseTexture> _offscreen_depth_rt1;
    ObjectPtr<IOpenGLBaseTexture> _offscreen_color_rt2;
    ObjectPtr<IOpenGLBaseTexture> _offscreen_depth_rt2;
    ObjectPtr<IOpenGLBaseTexture> _offscreen_color_rt3;
    ObjectPtr<IOpenGLBaseTexture> _offscreen_depth_rt3;

    Matrix4 _projection_matrix;
    Matrix4 _model_view_matrix;
    
    std::list<Matrix4>   _model_view_stack;
    std::list<BlendOperator> _blend_stack;

    //! The system GraphicsDisplay object
    GraphicsDisplay& _graphics_display;

    Rect _viewport;
    Rect _scissor;
    int _clip_offset_x;
    int _clip_offset_y;
    Rect _clipping_rect;


    //! Set to \i true to enable the global clipping rectangle.
    /*!
        When rendering directly to the OpenGL back buffer, we may enable a global clipping
        rectangle. This is mostly useful in embedded mode.
        \sa _global_clipping_rect;
    */
    bool _global_clipping_enabled;

    //! Define a global clipping rectangle.
    Rect _global_clipping_rect;

    std::list<Point> _clip_offset_stack;

    FontRenderer* _font_renderer;

    //Statistics
    mutable long m_quad_stats;
    mutable long m_quad_tex_stats;
    mutable long m_triangle_stats;
    mutable long m_triangle_tex_stats;
    mutable long m_line_stats;

    GraphicsEngine(const GraphicsEngine&);
    // Does not make sense for a singleton. This is a self assignment.
    GraphicsEngine& operator = (const GraphicsEngine&);
    // Declare operator address-of as private
    GraphicsEngine* operator & ();
  };

}

#endif // OPENGLENGINE_H

