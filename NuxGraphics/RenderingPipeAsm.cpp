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


#include "GLResource.h"
#include "IOpenGLBaseTexture.h"
#include "RenderingPipe.h"
//#include "RenderingPipeAsm.h"
#include "GraphicsEngine.h"

namespace nux
{

// Conventional Attribute Binding      Generic Attribute Binding
// ------------------------------      -------------------------
// vertex.position                     vertex.attrib[0]
// vertex.weight                       vertex.attrib[1]
// vertex.weight[0]                    vertex.attrib[1]
// vertex.normal                       vertex.attrib[2]
// vertex.color                        vertex.attrib[3]
// vertex.color.primary                vertex.attrib[3]
// vertex.color.secondary              vertex.attrib[4]
// vertex.fogcoord                     vertex.attrib[5]
// vertex.texcoord                     vertex.attrib[8]
// vertex.texcoord[0]                  vertex.attrib[8]
// vertex.texcoord[1]                  vertex.attrib[9]
// vertex.texcoord[2]                  vertex.attrib[10]
// vertex.texcoord[3]                  vertex.attrib[11]
// vertex.texcoord[4]                  vertex.attrib[12]
// vertex.texcoord[5]                  vertex.attrib[13]
// vertex.texcoord[6]                  vertex.attrib[14]
// vertex.texcoord[7]                  vertex.attrib[15]
// vertex.texcoord[n]                  vertex.attrib[8+n]

// # Vertex Attribute Registers
// # attribute Register                Components      Underlying State
// # vertex.position                   (x,y,z,w)       object position
// # vertex.weight                     (w,w,w,w)       vertex weights 0-3
// # vertex.weight[n]                  (w,w,w,w)       vertex weights n-n+3
// # vertex.normal                     (x,y,z,1)       NORMAL
// # vertex.color                      (r,g,b,a)       primary color
// # vertex.color.primary              (r,g,b,a)       primary color
// # vertex.color.secondary            (r,g,b,a)       secondary color
// # vertex.fogcoord                   (f,0,0,1)       fog coordinate
// # vertex.texcoord                   (s,t,r,q)       texture coordinate, unit 0
// # vertex.texcoord[n]                (s,t,r,q)       texture coordinate, unit n
// # vertex.matrixindex                (i,i,i,i)       vertex matrix indices 0-3
// # vertex.matrixindex[n]             (i,i,i,i)       vertex matrix indices n-n+3
// # vertex.attrib[n]                  (x,y,z,w)       generic vertex attribute n
//
//
// # Result Register                   Components      Description
// # result.position                   (x,y,z,w)       position in clip coordinates
// # result.color                      (r,g,b,a)       front-facing, primary color
// # result.color.primary              (r,g,b,a)       front-facing, primary color
// # result.color.secondary            (r,g,b,a)       front-facing, secondary color
// # result.color.front                (r,g,b,a)       front-facing, primary color
// # result.color.front.primary        (r,g,b,a)       front-facing, primary color
// # result.color.front.secondary      (r,g,b,a)       front-facing, secondary color
// # result.color.back                 (r,g,b,a)       back-facing, primary color
// # result.color.back.primary         (r,g,b,a)       back-facing, primary color
// # result.color.back.secondary       (r,g,b,a)       back-facing, secondary color
// # result.fogcoord                   (f,*,*,*)       fog coordinate
// # result.pointsize                  (s,*,*,*)       point size
// # result.texcoord                   (s,t,r,q)       texture coordinate, unit 0
// # result.texcoord[n]                (s,t,r,q)       texture coordinate, unit n

// # Fragment Attribute Binding  Components  Underlying State
// # --------------------------  ----------  ----------------------------
// # fragment.color              (r,g,b,a)   primary color
// # fragment.color.primary      (r,g,b,a)   primary color
// # fragment.color.secondary    (r,g,b,a)   secondary color
// # fragment.texcoord           (s,t,r,q)   texture coordinate, unit 0
// # fragment.texcoord[n]        (s,t,r,q)   texture coordinate, unit n
// # fragment.fogcoord           (f,0,0,1)   fog distance/coordinate
// # fragment.position           (x,y,z,1/w) window position


// # Fragment Program Results       Components  Description
// # -----------------------------  ----------  ----------------------------
// # result.color                   (r,g,b,a)   color
// # result.depth                   (*,*,d,*)   depth coordinate


  void GraphicsEngine::InitAsmColorShader()
  {
    NString AsmVtx = TEXT (
        "!!ARBvp1.0                                 \n\
        ATTRIB iPos         = vertex.position;      \n\
        ATTRIB iColor       = vertex.attrib[3];     \n\
        PARAM  mvp[4]       = {state.matrix.mvp};   \n\
        OUTPUT oPos         = result.position;      \n\
        OUTPUT oColor       = result.color;         \n\
        # Transform the vertex to clip coordinates. \n\
        DP4   oPos.x, mvp[0], iPos;      \n\
        DP4   oPos.y, mvp[1], iPos;      \n\
        DP4   oPos.z, mvp[2], iPos;      \n\
        DP4   oPos.w, mvp[3], iPos;      \n\
        MOV   oColor, iColor;            \n\
        END");

    NString AsmFrg = TEXT (
        "!!ARBfp1.0                                     \n\
        MOV result.color, fragment.color;               \n\
        END");

    m_AsmColor = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_AsmColor->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_AsmColor->LoadPixelShader (AsmFrg.GetTCharPtr() );
    m_AsmColor->Link();
  }

  void GraphicsEngine::InitAsmTextureShader()
  {
    NString AsmVtx = TEXT (
        "!!ARBvp1.0                                 \n\
        ATTRIB iPos         = vertex.position;      \n\
        ATTRIB iColor       = vertex.attrib[3];     \n\
        PARAM  mvp[4]       = {state.matrix.mvp};   \n\
        OUTPUT oPos         = result.position;      \n\
        OUTPUT oColor       = result.color;         \n\
        OUTPUT oTexCoord0   = result.texcoord[0];   \n\
        # Transform the vertex to clip coordinates. \n\
        DP4   oPos.x, mvp[0], iPos;                     \n\
        DP4   oPos.y, mvp[1], iPos;                     \n\
        DP4   oPos.z, mvp[2], iPos;                     \n\
        DP4   oPos.w, mvp[3], iPos;                     \n\
        MOV   oColor, iColor;                           \n\
        MOV   oTexCoord0, vertex.attrib[8];             \n\
        END");

    NString AsmFrg = TEXT (
        "!!ARBfp1.0                                       \n\
        TEMP tex0;                                        \n\
        TEX tex0, fragment.texcoord[0], texture[0], 2D;   \n\
        MUL result.color, fragment.color, tex0;           \n\
        END");

    NString AsmFrgRect = TEXT (
      "!!ARBfp1.0                                       \n\
      TEMP tex0;                                        \n\
      TEX tex0, fragment.texcoord[0], texture[0], RECT; \n\
      MUL result.color, fragment.color, tex0;           \n\
      END");

    m_AsmTextureModColor = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_AsmTextureModColor->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_AsmTextureModColor->LoadPixelShader (AsmFrg.GetTCharPtr() );
    m_AsmTextureModColor->Link();

    m_AsmTextureRectModColor = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_AsmTextureRectModColor->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_AsmTextureRectModColor->LoadPixelShader (AsmFrgRect.GetTCharPtr() );
    m_AsmTextureRectModColor->Link();
  }

  void GraphicsEngine::InitAsmColorModTexMaskAlpha()
  {
    NString AsmVtx = TEXT (
        "!!ARBvp1.0                                 \n\
        OUTPUT oPos         = result.position;      \n\
        OUTPUT oColor       = result.color;         \n\
        OUTPUT oTexCoord0   = result.texcoord[0];   \n\
        # Transform the vertex to clip coordinates. \n\
        DP4   oPos.x, state.matrix.mvp.row[0], vertex.position;      \n\
        DP4   oPos.y, state.matrix.mvp.row[1], vertex.position;      \n\
        DP4   oPos.z, state.matrix.mvp.row[2], vertex.position;      \n\
        DP4   oPos.w, state.matrix.mvp.row[3], vertex.position;      \n\
        MOV   oColor, vertex.attrib[3];             \n\
        MOV   oTexCoord0, vertex.attrib[8];         \n\
        END");

    NString AsmFrg = TEXT (
      "!!ARBfp1.0                                         \n\
      TEMP tex0;                                          \n\
      TEMP temp;                                          \n\
      TEX tex0, fragment.texcoord[0], texture[0], 2D;     \n\
      MOV temp, fragment.color;                           \n\
      MUL temp.w, fragment.color, tex0.wwww;              \n\
      MOV result.color, temp;                             \n\
      END");

    NString AsmFrgRect = TEXT (
      "!!ARBfp1.0                                         \n\
      TEMP tex0;                                          \n\
      TEMP temp;                                          \n\
      TEX tex0, fragment.texcoord[0], texture[0], RECT;   \n\
      MOV temp, fragment.color;                           \n\
      MUL temp.w, fragment.color, tex0.wwww;              \n\
      MOV result.color, temp;                             \n\
      END");

    m_AsmColorModTexMaskAlpha = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_AsmColorModTexMaskAlpha->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_AsmColorModTexMaskAlpha->LoadPixelShader (AsmFrg.GetTCharPtr() );
    m_AsmColorModTexMaskAlpha->Link();

    m_AsmColorModTexRectMaskAlpha = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_AsmColorModTexRectMaskAlpha->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_AsmColorModTexRectMaskAlpha->LoadPixelShader (AsmFrgRect.GetTCharPtr() );
    m_AsmColorModTexRectMaskAlpha->Link();
  }

  void GraphicsEngine::InitAsm2TextureAdd()
  {
    NString AsmVtx = TEXT (
        "!!ARBvp1.0                                 \n\
        ATTRIB iPos         = vertex.position;      \n\
        OUTPUT oPos         = result.position;      \n\
        OUTPUT oTexCoord0   = result.texcoord[0];   \n\
        OUTPUT oTexCoord1   = result.texcoord[1];   \n\
        # Transform the vertex to clip coordinates. \n\
        DP4   oPos.x, state.matrix.mvp.row[0], vertex.position;      \n\
        DP4   oPos.y, state.matrix.mvp.row[1], vertex.position;      \n\
        DP4   oPos.z, state.matrix.mvp.row[2], vertex.position;      \n\
        DP4   oPos.w, state.matrix.mvp.row[3], vertex.position;      \n\
        MOV   oTexCoord0, vertex.attrib[8];       \n\
        MOV   oTexCoord1, vertex.attrib[9];       \n\
        END");

    NString AsmFrg = TEXT (
        "!!ARBfp1.0                                         \n\
        PARAM color0 = program.local[0];                    \n\
        PARAM color1 = program.local[1];                    \n\
        TEMP tex0;                                          \n\
        TEMP tex1;                                          \n\
        TEMP temp;                                          \n\
        TEX tex0, fragment.texcoord[0], texture[0], 2D;     \n\
        MUL temp, color0, tex0;                             \n\
        TEX tex1, fragment.texcoord[1], texture[1], 2D;     \n\
        MAD temp, color1, tex1, temp;                       \n\
        MOV result.color, temp;                             \n\
        END");

    NString AsmFrgRect = TEXT (
      "!!ARBfp1.0                                         \n\
      PARAM color0 = program.local[0];                    \n\
      PARAM color1 = program.local[1];                    \n\
      TEMP tex0;                                          \n\
      TEMP tex1;                                          \n\
      TEMP temp;                                          \n\
      TEX tex0, fragment.texcoord[0], texture[0], RECT;   \n\
      MUL temp, color0, tex0;                             \n\
      TEX tex1, fragment.texcoord[1], texture[1], RECT;   \n\
      MAD temp, color1, tex1, temp;                       \n\
      MOV result.color, temp;                             \n\
      END");

    m_Asm2TextureAdd = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_Asm2TextureAdd->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_Asm2TextureAdd->LoadPixelShader (AsmFrg.GetTCharPtr() );
    m_Asm2TextureAdd->Link();

    m_Asm2TextureRectAdd = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_Asm2TextureRectAdd->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_Asm2TextureRectAdd->LoadPixelShader (AsmFrgRect.GetTCharPtr() );
    m_Asm2TextureRectAdd->Link();
  }

  void GraphicsEngine::InitAsm4TextureAdd()
  {
    NString AsmVtx = TEXT (
        "!!ARBvp1.0                                 \n\
        ATTRIB iPos         = vertex.position;      \n\
        OUTPUT oPos         = result.position;      \n\
        OUTPUT oTexCoord0   = result.texcoord[0];   \n\
        OUTPUT oTexCoord1   = result.texcoord[1];   \n\
        OUTPUT oTexCoord2   = result.texcoord[2];   \n\
        OUTPUT oTexCoord3   = result.texcoord[3];   \n\
        # Transform the vertex to clip coordinates. \n\
        DP4   oPos.x, state.matrix.mvp.row[0], vertex.position;      \n\
        DP4   oPos.y, state.matrix.mvp.row[1], vertex.position;      \n\
        DP4   oPos.z, state.matrix.mvp.row[2], vertex.position;      \n\
        DP4   oPos.w, state.matrix.mvp.row[3], vertex.position;      \n\
        MOV   oTexCoord0, vertex.attrib[8];       \n\
        MOV   oTexCoord1, vertex.attrib[9];       \n\
        MOV   oTexCoord2, vertex.attrib[10];       \n\
        MOV   oTexCoord3, vertex.attrib[11];       \n\
        END");

    NString AsmFrg = TEXT (
        "!!ARBfp1.0                                         \n\
        PARAM color0 = program.local[0];                    \n\
        PARAM color1 = program.local[1];                    \n\
        PARAM color2 = program.local[2];                    \n\
        PARAM color3 = program.local[3];                    \n\
        TEMP tex0;                                          \n\
        TEMP tex1;                                          \n\
        TEMP tex2;                                          \n\
        TEMP tex3;                                          \n\
        TEMP temp;                                          \n\
        TEX tex0, fragment.texcoord[0], texture[0], 2D;     \n\
        MUL temp, color0, tex0;                             \n\
        TEX tex1, fragment.texcoord[1], texture[1], 2D;     \n\
        MAD temp, color1, tex1, temp;                       \n\
        TEX tex2, fragment.texcoord[2], texture[2], 2D;     \n\
        MAD temp, color2, tex2, temp;                       \n\
        TEX tex3, fragment.texcoord[3], texture[3], 2D;     \n\
        MAD temp, color3, tex3, temp;                       \n\
        MOV result.color, temp;                             \n\
        END");

    NString AsmFrgRect = TEXT (
      "!!ARBfp1.0                                         \n\
      PARAM color0 = program.local[0];                    \n\
      PARAM color1 = program.local[1];                    \n\
      PARAM color2 = program.local[2];                    \n\
      PARAM color3 = program.local[3];                    \n\
      TEMP tex0;                                          \n\
      TEMP tex1;                                          \n\
      TEMP tex2;                                          \n\
      TEMP tex3;                                          \n\
      TEMP temp;                                          \n\
      TEX tex0, fragment.texcoord[0], texture[0], RECT;   \n\
      MUL temp, color0, tex0;                             \n\
      TEX tex1, fragment.texcoord[1], texture[1], RECT;   \n\
      MAD temp, color1, tex1, temp;                       \n\
      TEX tex2, fragment.texcoord[2], texture[2], RECT;   \n\
      MAD temp, color2, tex2, temp;                       \n\
      TEX tex3, fragment.texcoord[3], texture[3], RECT;   \n\
      MAD temp, color3, tex3, temp;                       \n\
      MOV result.color, temp;                             \n\
      END");

    m_Asm4TextureAdd = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_Asm4TextureAdd->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_Asm4TextureAdd->LoadPixelShader (AsmFrg.GetTCharPtr() );
    m_Asm4TextureAdd->Link();

    m_Asm4TextureRectAdd = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_Asm4TextureRectAdd->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_Asm4TextureRectAdd->LoadPixelShader (AsmFrgRect.GetTCharPtr() );
    m_Asm4TextureRectAdd->Link();
  }

  void GraphicsEngine::InitAsmBlendModes()
  {
    NString AsmVtx = TEXT (
                       "!!ARBvp1.0                                 \n\
        OUTPUT oPos         = result.position;      \n\
        OUTPUT oTexCoord0   = result.texcoord[0];   \n\
        OUTPUT oTexCoord1   = result.texcoord[1];   \n\
        # Transform the vertex to clip coordinates. \n\
        DP4   oPos.x, state.matrix.mvp.row[0], vertex.position;      \n\
        DP4   oPos.y, state.matrix.mvp.row[1], vertex.position;      \n\
        DP4   oPos.z, state.matrix.mvp.row[2], vertex.position;      \n\
        DP4   oPos.w, state.matrix.mvp.row[3], vertex.position;      \n\
        MOV   oTexCoord0, vertex.attrib[8];       \n\
        MOV   oTexCoord1, vertex.attrib[9];       \n\
        END");

    NString AsmPSBNormal = TEXT (
                             "!!ARBfp1.0                                         \n\
        TEMP tex0;                                          \n\
        TEMP tex1;                                          \n\
        TEX tex0, fragment.texcoord[0], texture[0], 2D;     \n\
        TEX tex1, fragment.texcoord[1], texture[1], 2D;     \n\
        MOV result.color, tex0;                             \n\
        END");

    m_AsmPSBNormal = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_AsmPSBNormal->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_AsmPSBNormal->LoadPixelShader (AsmPSBNormal.GetTCharPtr() );
    m_AsmPSBNormal->Link();

    // Lighten
    NString AsmPSBLighten = TEXT (
                              "!!ARBfp1.0                                         \n\
        TEMP tex0;                                          \n\
        TEMP tex1;                                          \n\
        TEMP temp;                                          \n\
        TEMP cmpres;                                        \n\
        TEX tex0, fragment.texcoord[0], texture[0], 2D;     \n\
        TEX tex1, fragment.texcoord[1], texture[1], 2D;     \n\
        SUB temp, tex0, tex1;                               \n\
        CMP cmpres, temp, tex1, tex0;                       \n\
        MOV result.color, temp;                             \n\
        END");

    m_AsmPSBLighten = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_AsmPSBLighten->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_AsmPSBLighten->LoadPixelShader (AsmPSBLighten.GetTCharPtr() );
    m_AsmPSBLighten->Link();

    // Darken
    NString AsmPSBDarken = TEXT (
                             "!!ARBfp1.0                                         \n\
        TEMP tex0;                                          \n\
        TEMP tex1;                                          \n\
        TEMP temp;                                          \n\
        TEMP cmpres;                                        \n\
        TEX tex0, fragment.texcoord[0], texture[0], 2D;     \n\
        TEX tex1, fragment.texcoord[1], texture[1], 2D;     \n\
        SUB temp, tex1, tex0;                               \n\
        CMP cmpres, temp, tex1, tex0;                       \n\
        MOV result.color, temp;                             \n\
        END");

    m_AsmPSBDarken = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_AsmPSBDarken->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_AsmPSBDarken->LoadPixelShader (AsmPSBDarken.GetTCharPtr() );
    m_AsmPSBDarken->Link();

    // Multiply
    NString AsmPSBMultiply = TEXT (
                               "!!ARBfp1.0                                         \n\
        TEMP tex0;                                          \n\
        TEMP tex1;                                          \n\
        TEMP temp;                                          \n\
        TEX tex0, fragment.texcoord[0], texture[0], 2D;     \n\
        TEX tex1, fragment.texcoord[1], texture[1], 2D;     \n\
        MUL result.color, tex0, tex1;                       \n\
        END");

    m_AsmPSBMultiply = GetThreadGLDeviceFactory()->CreateAsmShaderProgram();
    m_AsmPSBMultiply->LoadVertexShader (AsmVtx.GetTCharPtr() );
    m_AsmPSBMultiply->LoadPixelShader (AsmPSBMultiply.GetTCharPtr() );
    m_AsmPSBMultiply->Link();
  }

  void GraphicsEngine::QRP_Color (int x, int y, int width, int height, const Color &c0)
  {
    QRP_Color (x, y, width, height, c0, c0, c0, c0);
  }

  void GraphicsEngine::QRP_Color (int x, int y, int width, int height, const Color &c0, const Color &c1, const Color &c2, const Color &c3)
  {
    float VtxBuffer[] =
    {
      x,          y,          0.0f, 1.0f, c0.R(), c0.G(), c0.B(), c0.A(),
      x,          y + height, 0.0f, 1.0f, c1.R(), c1.G(), c1.B(), c1.A(),
      x + width,  y + height, 0.0f, 1.0f, c2.R(), c2.G(), c2.B(), c2.A(),
      x + width,  y,          0.0f, 1.0f, c3.R(), c3.G(), c3.B(), c3.A(),
    };

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );

    IntrusiveSP<IOpenGLAsmShaderProgram> shader_program = m_AsmColor;

    shader_program->Begin();

    CHECKGL ( glMatrixMode (GL_MODELVIEW) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetModelViewMatrix().m) );
    CHECKGL ( glMatrixMode (GL_PROJECTION) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetProjectionMatrix().m) );


    int VertexLocation          = VTXATTRIB_POSITION;
    int VertexColorLocation     = VTXATTRIB_COLOR;

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer) );

    if (VertexColorLocation != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (VertexColorLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4) );
    }

    CHECKGL ( glDrawArrays (GL_QUADS, 0, 4) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (VertexColorLocation != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (VertexColorLocation) );

    shader_program->End();
  }

  void GraphicsEngine::QRP_1Tex (int x, int y, int width, int height, IntrusiveSP<IOpenGLBaseTexture> device_texture, TexCoordXForm texxform, Color color)
  {
    QRP_Compute_Texture_Coord (width, height, device_texture, texxform);
    float VtxBuffer[] =
    {
      x,          y,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 1.0f, color.R(), color.G(), color.B(), color.A(),
      x,          y + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 1.0f, color.R(), color.G(), color.B(), color.A(),
      x + width,  y + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 1.0f, color.R(), color.G(), color.B(), color.A(),
      x + width,  y,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 1.0f, color.R(), color.G(), color.B(), color.A(),
    };

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
    
    IntrusiveSP<IOpenGLAsmShaderProgram> shader_program = m_AsmTextureModColor;
    if(device_texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = m_AsmTextureRectModColor;
    }
    shader_program->Begin();

    SetTexture (GL_TEXTURE0, device_texture);

    CHECKGL ( glMatrixMode (GL_MODELVIEW) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetModelViewMatrix().m) );
    CHECKGL ( glMatrixMode (GL_PROJECTION) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetProjectionMatrix().m) );


    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int VertexColorLocation     = VTXATTRIB_COLOR;

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer) );

    if (TextureCoord0Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord0Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4) );
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (VertexColorLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8) );
    }

    CHECKGL ( glDrawArrays (GL_QUADS, 0, 4) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (TextureCoord0Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord0Location) );

    if (VertexColorLocation != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (VertexColorLocation) );

    shader_program->End();
  }

  void GraphicsEngine::QRP_ColorModTexAlpha (int x, int y, int width, int height,
      IntrusiveSP<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &color)
  {
    QRP_Compute_Texture_Coord (width, height, device_texture, texxform);

    float VtxBuffer[] =
    {
      x,          y,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 1.0f, color.R(), color.G(), color.B(), color.A(),
      x,          y + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 1.0f, color.R(), color.G(), color.B(), color.A(),
      x + width,  y + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 1.0f, color.R(), color.G(), color.B(), color.A(),
      x + width,  y,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 1.0f, color.R(), color.G(), color.B(), color.A(),
    };

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );

    IntrusiveSP<IOpenGLAsmShaderProgram> shader_program = m_AsmColorModTexMaskAlpha;
    if(device_texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = m_AsmColorModTexRectMaskAlpha;
    }
    shader_program->Begin();

    SetTexture (GL_TEXTURE0, device_texture);

    CHECKGL ( glMatrixMode (GL_MODELVIEW) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetModelViewMatrix().m) );
    CHECKGL ( glMatrixMode (GL_PROJECTION) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetProjectionMatrix().m) );


    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int VertexColorLocation     = VTXATTRIB_COLOR;

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer) );

    if (TextureCoord0Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord0Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4) );
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (VertexColorLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8) );
    }

    CHECKGL ( glDrawArrays (GL_QUADS, 0, 4) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (TextureCoord0Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord0Location) );

    if (VertexColorLocation != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (VertexColorLocation) );

    shader_program->End();
  }

  void GraphicsEngine::QRP_2Tex (int x, int y, int width, int height,
                                  IntrusiveSP<IOpenGLBaseTexture> device_texture0, TexCoordXForm &texxform0, const Color &color0,
                                  IntrusiveSP<IOpenGLBaseTexture> device_texture1, TexCoordXForm &texxform1, const Color &color1)
  {
    QRP_Compute_Texture_Coord (width, height, device_texture0, texxform0);
    QRP_Compute_Texture_Coord (width, height, device_texture1, texxform1);

    float VtxBuffer[] =
    {
      x,          y,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0.0f, 1.0f, texxform1.u0, texxform1.v0, 0.0f, 1.0f,
      x,          y + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0.0f, 1.0f, texxform1.u0, texxform1.v1, 0.0f, 1.0f,
      x + width,  y + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0.0f, 1.0f, texxform1.u1, texxform1.v1, 0.0f, 1.0f,
      x + width,  y,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0.0f, 1.0f, texxform1.u1, texxform1.v0, 0.0f, 1.0f,
    };

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );

    IntrusiveSP<IOpenGLAsmShaderProgram> shader_program = m_Asm2TextureAdd;
    if(device_texture0->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = m_Asm2TextureRectAdd;
    }
    shader_program->Begin();

    SetTexture (GL_TEXTURE0, device_texture0);
    SetTexture (GL_TEXTURE0, device_texture1);

    CHECKGL ( glMatrixMode (GL_MODELVIEW) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetModelViewMatrix().m) );
    CHECKGL ( glMatrixMode (GL_PROJECTION) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetProjectionMatrix().m) );

    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int TextureCoord1Location   = VTXATTRIB_TEXCOORD1;

    CHECKGL ( glProgramLocalParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 0, color0.R(), color0.G(), color0.B(), color0.A() ) );
    CHECKGL ( glProgramLocalParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 1, color1.R(), color1.G(), color1.B(), color1.A() ) );

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer) );

    if (TextureCoord0Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord0Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4) );
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord1Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8) );
    }

    CHECKGL ( glDrawArrays (GL_QUADS, 0, 4) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (TextureCoord0Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord0Location) );

    if (TextureCoord1Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord1Location) );

    shader_program->End();
  }

  void GraphicsEngine::QRP_4Tex (int x, int y, int width, int height,
                                  IntrusiveSP<IOpenGLBaseTexture> device_texture0, TexCoordXForm &texxform0, const Color &color0,
                                  IntrusiveSP<IOpenGLBaseTexture> device_texture1, TexCoordXForm &texxform1, const Color &color1,
                                  IntrusiveSP<IOpenGLBaseTexture> device_texture2, TexCoordXForm &texxform2, const Color &color2,
                                  IntrusiveSP<IOpenGLBaseTexture> device_texture3, TexCoordXForm &texxform3, const Color &color3)
  {
    QRP_Compute_Texture_Coord (width, height, device_texture0, texxform0);
    QRP_Compute_Texture_Coord (width, height, device_texture1, texxform1);
    QRP_Compute_Texture_Coord (width, height, device_texture2, texxform1);
    QRP_Compute_Texture_Coord (width, height, device_texture3, texxform1);

    float VtxBuffer[] =
    {
      x,          y,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 1.0f, texxform1.u0, texxform1.v0, 0, 1.0f, texxform2.u0, texxform2.v0, 0, 1.0f, texxform3.u0, texxform3.v0, 0, 1.0f,
      x,          y + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 1.0f, texxform1.u0, texxform1.v1, 0, 1.0f, texxform2.u0, texxform2.v1, 0, 1.0f, texxform3.u0, texxform3.v1, 0, 1.0f,
      x + width,  y + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 1.0f, texxform1.u1, texxform1.v1, 0, 1.0f, texxform2.u1, texxform2.v1, 0, 1.0f, texxform3.u1, texxform3.v1, 0, 1.0f,
      x + width,  y,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 1.0f, texxform1.u1, texxform1.v0, 0, 1.0f, texxform2.u1, texxform2.v0, 0, 1.0f, texxform3.u1, texxform3.v0, 0, 1.0f,
    };

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );

    IntrusiveSP<IOpenGLAsmShaderProgram> shader_program = m_Asm4TextureAdd;
    if(device_texture0->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = m_Asm4TextureRectAdd;
    }
    shader_program->Begin();

    SetTexture (GL_TEXTURE0, device_texture0);
    SetTexture (GL_TEXTURE0, device_texture1);

    CHECKGL ( glMatrixMode (GL_MODELVIEW) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetModelViewMatrix().m) );
    CHECKGL ( glMatrixMode (GL_PROJECTION) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetProjectionMatrix().m) );

    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int TextureCoord1Location   = VTXATTRIB_TEXCOORD1;
    int TextureCoord2Location   = VTXATTRIB_TEXCOORD2;
    int TextureCoord3Location   = VTXATTRIB_TEXCOORD3;

    CHECKGL ( glProgramLocalParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 0, color0.R(), color0.G(), color0.B(), color0.A() ) );
    CHECKGL ( glProgramLocalParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 1, color1.R(), color1.G(), color1.B(), color1.A() ) );
    CHECKGL ( glProgramLocalParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 2, color2.R(), color2.G(), color2.B(), color2.A() ) );
    CHECKGL ( glProgramLocalParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 3, color3.R(), color3.G(), color3.B(), color3.A() ) );

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer) );

    if (TextureCoord0Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord0Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 4) );
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord1Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 8) );
    }

    if (TextureCoord2Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord2Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord2Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 12) );
    }

    if (TextureCoord3Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord3Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord3Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 16) );
    }

    CHECKGL ( glDrawArrays (GL_QUADS, 0, 4) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (TextureCoord0Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord0Location) );

    if (TextureCoord1Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord1Location) );

    if (TextureCoord2Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord2Location) );

    if (TextureCoord3Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord3Location) );

    shader_program->End();
  }

////////////////////////////////////////////////////////////////////////////////////
  void GraphicsEngine::QRP_Triangle (int x0, int y0,
                                      int x1, int y1,
                                      int x2, int y2,
                                      Color c0)
  {
    QRP_Triangle (x0, y0, x1, y1, x2, y2, c0, c0, c0);
  }

  void GraphicsEngine::QRP_Triangle (int x0, int y0,
                                      int x1, int y1,
                                      int x2, int y2,
                                      Color c0, Color c1, Color c2)
  {
    float VtxBuffer[] =
    {
      x0, y0, 0.0f, 1.0f, c0.R(), c0.G(), c0.B(), c0.A(),
      x1, y1, 0.0f, 1.0f, c1.R(), c1.G(), c1.B(), c1.A(),
      x2, y2, 0.0f, 1.0f, c2.R(), c2.G(), c2.B(), c2.A(),
    };

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );

    IntrusiveSP<IOpenGLAsmShaderProgram> ShaderProg = m_AsmColor;

    ShaderProg->Begin();

    CHECKGL ( glMatrixMode (GL_MODELVIEW) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetModelViewMatrix().m) );
    CHECKGL ( glMatrixMode (GL_PROJECTION) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetProjectionMatrix().m) );

    int VertexLocation          = VTXATTRIB_POSITION;
    int VertexColorLocation     = VTXATTRIB_COLOR;

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer) );

    CHECKGL ( glEnableVertexAttribArrayARB (VertexColorLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4) );

    CHECKGL ( glDrawArrays (GL_TRIANGLES, 0, 3) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glDisableVertexAttribArrayARB (VertexColorLocation) );
    ShaderProg->End();

    m_triangle_stats++;
  }

//////////////////////
// DRAW LINES       //
//////////////////////
  void GraphicsEngine::QRP_Line (int x0, int y0,
                                  int x1, int y1, Color c0)
  {
    QRP_Line (x0, y0, x1, y1, c0, c0);
  }

  void GraphicsEngine::QRP_Line (int x0, int y0,
                                  int x1, int y1, Color c0, Color c1)
  {

    float VtxBuffer[] =
    {
      x0, y0, 0.0f, 1.0f, c0.R(), c0.G(), c0.B(), c0.A(),
      x1, y1, 0.0f, 1.0f, c1.R(), c1.G(), c1.B(), c1.A(),
    };

    IntrusiveSP<IOpenGLAsmShaderProgram> ShaderProg = m_AsmColor;


    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
    ShaderProg->Begin();

    CHECKGL ( glMatrixMode (GL_MODELVIEW) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetModelViewMatrix().m) );
    CHECKGL ( glMatrixMode (GL_PROJECTION) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetProjectionMatrix().m) );

    int VertexLocation          = VTXATTRIB_POSITION;
    int VertexColorLocation     = VTXATTRIB_COLOR;

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer) );

    if (VertexColorLocation != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (VertexColorLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4) );
    }

    CHECKGL ( glDrawArrays (GL_LINES, 0, 2) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (VertexColorLocation != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (VertexColorLocation) );

    ShaderProg->End();

    m_line_stats++;
  }

  void GraphicsEngine::QRP_QuadWireframe (int x0, int y0, int width, int height,
      Color c0,
      Color c1,
      Color c2,
      Color c3)
  {
    float VtxBuffer[] =
    {
      x0, y0,                             0.0f, 1.0f, c0.R(), c0.G(), c0.B(), c0.A(),
      x0, y0 + height - 1,                0.0f, 1.0f, c1.R(), c1.G(), c1.B(), c1.A(),
      x0 + width - 1, y0 + height - 1,    0.0f, 1.0f, c2.R(), c2.G(), c2.B(), c2.A(),
      x0 + width - 1, y0,                 0.0f, 1.0f, c3.R(), c3.G(), c3.B(), c3.A(),
      x0, y0,                             0.0f, 1.0f, c0.R(), c0.G(), c0.B(), c0.A(),
    };

    IntrusiveSP<IOpenGLAsmShaderProgram> ShaderProg = m_AsmColor;

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
    ShaderProg->Begin();

    CHECKGL ( glMatrixMode (GL_MODELVIEW) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetModelViewMatrix().m) );
    CHECKGL ( glMatrixMode (GL_PROJECTION) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) GetProjectionMatrix().m) );

    int VertexLocation          = VTXATTRIB_POSITION;
    int VertexColorLocation     = VTXATTRIB_COLOR;

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer) );

    if (VertexColorLocation != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (VertexColorLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4) );
    }

    CHECKGL ( glDrawArrays (GL_LINE_STRIP, 0, 5) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (VertexColorLocation != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (VertexColorLocation) );

    ShaderProg->End();

    m_line_stats++;
  }

}
