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

#ifndef NUX_OPENGLES_20

#include "GLResource.h"
#include "IOpenGLBaseTexture.h"
#include "RenderingPipe.h"
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
// # result.color.front                (r,g,b,a)       front-facing, primary color
// # result.color.front.primary        (r,g,b,a)       front-facing, primary color
// # result.color.front.secondary      (r,g,b,a)       front-facing, secondary color
// # result.color.back                 (r,g,b,a)       back-facing, primary color
// # result.color.back.primary         (r,g,b,a)       back-facing, primary color
// # result.color.back.secondary       (r,g,b,a)       back-facing, secondary color
// # result.fogcoord                   (f,*,*,*)       fog coordinate
// # result.pointsize                  (s,*,*,*)       point size
// # result.texcoord                   (s,t,r,q)       texture coordinate, unit 0
// # result.color.secondary            (r,g,b,a)       front-facing, secondary color
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
    const char* AsmVtx = 
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
        END";

    const char* AsmFrg = 
        "!!ARBfp1.0                                     \n\
        MOV result.color, fragment.color;               \n\
        END";

    m_AsmColor = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmColor->LoadVertexShader(AsmVtx);
    m_AsmColor->LoadPixelShader(AsmFrg);
    m_AsmColor->Link();
  }

  void GraphicsEngine::InitAsmTextureShader()
  {
    const char* AsmVtx = 
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
        END";

    const char* AsmFrg = 
        "!!ARBfp1.0                                       \n\
        TEMP tex0;                                        \n\
        TEX tex0, fragment.texcoord[0], texture[0], 2D;   \n\
        MUL result.color, fragment.color, tex0;           \n\
        END";

    const char* AsmFrgRect = 
      "!!ARBfp1.0                                       \n\
      TEMP tex0;                                        \n\
      TEX tex0, fragment.texcoord[0], texture[0], RECT; \n\
      MUL result.color, fragment.color, tex0;           \n\
      END";

    m_AsmTextureModColor = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmTextureModColor->LoadVertexShader(AsmVtx);
    m_AsmTextureModColor->LoadPixelShader(AsmFrg);
    m_AsmTextureModColor->Link();

    m_AsmTextureRectModColor = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmTextureRectModColor->LoadVertexShader(AsmVtx);
    m_AsmTextureRectModColor->LoadPixelShader(AsmFrgRect);
    m_AsmTextureRectModColor->Link();
  }

  void GraphicsEngine::InitAsmColorModTexMaskAlpha()
  {
    const char* AsmVtx = 
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
        END";

    const char* AsmFrg = 
      "!!ARBfp1.0                                         \n\
      TEMP tex0;                                          \n\
      TEMP temp;                                          \n\
      TEX tex0, fragment.texcoord[0], texture[0], 2D;     \n\
      MOV temp, fragment.color;                           \n\
      MUL temp.w, fragment.color, tex0.wwww;              \n\
      MOV result.color, temp;                             \n\
      END";

    const char* AsmFrgRect = 
      "!!ARBfp1.0                                         \n\
      TEMP tex0;                                          \n\
      TEMP temp;                                          \n\
      TEX tex0, fragment.texcoord[0], texture[0], RECT;   \n\
      MOV temp, fragment.color;                           \n\
      MUL temp.w, fragment.color, tex0.wwww;              \n\
      MOV result.color, temp;                             \n\
      END";

    m_AsmColorModTexMaskAlpha = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmColorModTexMaskAlpha->LoadVertexShader(AsmVtx);
    m_AsmColorModTexMaskAlpha->LoadPixelShader(AsmFrg);
    m_AsmColorModTexMaskAlpha->Link();

    m_AsmColorModTexRectMaskAlpha = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmColorModTexRectMaskAlpha->LoadVertexShader(AsmVtx);
    m_AsmColorModTexRectMaskAlpha->LoadPixelShader(AsmFrgRect);
    m_AsmColorModTexRectMaskAlpha->Link();
  }

  void GraphicsEngine::InitAsm2TextureAdd()
  {
    const char* AsmVtx = 
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
        END";

    const char* AsmFrg = 
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
        END";

    const char* AsmFrgRect = 
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
      END";

    m_Asm2TextureAdd = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_Asm2TextureAdd->LoadVertexShader(AsmVtx);
    m_Asm2TextureAdd->LoadPixelShader(AsmFrg);
    m_Asm2TextureAdd->Link();

    m_Asm2TextureRectAdd = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_Asm2TextureRectAdd->LoadVertexShader(AsmVtx);
    m_Asm2TextureRectAdd->LoadPixelShader(AsmFrgRect);
    m_Asm2TextureRectAdd->Link();
  }

  void GraphicsEngine::InitAsm2TextureDepRead()
  {
    const char* AsmVtx = 
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
      END";

    const char* AsmFrg = 
      "!!ARBfp1.0                                         \n\
      PARAM color0 = program.local[0];                    \n\
      PARAM color1 = program.local[1];                    \n\
      TEMP tex0;                                          \n\
      TEMP tex1;                                          \n\
      TEMP temp;                                          \n\
      TEMP temp0;                                          \n\
      TEMP temp1;                                          \n\
      TEX tex0, fragment.texcoord[0], texture[0], 2D;     \n\
      MAD temp, {2.0, 2.0, 2.0, 2.0}, tex0, {-1.0, -1.0, -1.0, -1.0}; \n\
      MUL temp0, color0, temp;                             \n\
      ADD temp1, texture[1];                         \n\
      TEX tex1, fragment.texcoord[1], texture[1], 2D;           \n\
      MUL result.color, color1, tex1;                     \n\
      END";

    const char* AsmFrgRect = 
      "!!ARBfp1.0                                         \n\
      PARAM color0 = program.local[0];                    \n\
      PARAM color1 = program.local[1];                    \n\
      TEMP tex0;                                          \n\
      TEMP tex1;                                          \n\
      TEMP temp;                                          \n\
      TEX tex0, fragment.texcoord[0], texture[0], RECT;   \n\
      MAD temp, {2.0, 2.0, 2.0, 2.0}, tex0, {-1.0, -1.0, -1.0, -1.0}; \n\
      MUL temp, color0, tex0;                             \n\
      TEX tex1, fragment.texcoord[1], temp, RECT;         \n\
      MUL result.color, color1, tex1;                     \n\
      END";

    m_ASM2TextureDepRead = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_ASM2TextureDepRead->LoadVertexShader(AsmVtx);
    m_ASM2TextureDepRead->LoadPixelShader(AsmFrg);
    m_ASM2TextureDepRead->Link();

    m_ASM2TextureRectDepRead = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_ASM2TextureRectDepRead->LoadVertexShader(AsmVtx);
    m_ASM2TextureRectDepRead->LoadPixelShader(AsmFrgRect);
    m_ASM2TextureRectDepRead->Link();
  }

  void GraphicsEngine::InitAsm2TextureMod()
  {
    const char* AsmVtx = 
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
      END";

    const char* AsmFrg = 
      "!!ARBfp1.0                                         \n\
      PARAM color0 = program.local[0];                    \n\
      PARAM color1 = program.local[1];                    \n\
      TEMP tex0;                                          \n\
      TEMP tex1;                                          \n\
      TEMP temp0;                                         \n\
      TEMP temp1;                                         \n\
      TEX tex0, fragment.texcoord[0], texture[0], 2D;     \n\
      MUL temp0, color0, tex0;                            \n\
      TEX tex1, fragment.texcoord[1], texture[1], 2D;     \n\
      MUL temp1, color1, tex1;                            \n\
      MUL result.color, temp0, temp1;                     \n\
      END";

    const char* AsmFrgRect = 
      "!!ARBfp1.0                                         \n\
      PARAM color0 = program.local[0];                    \n\
      PARAM color1 = program.local[1];                    \n\
      TEMP tex0;                                          \n\
      TEMP tex1;                                          \n\
      TEMP temp0;                                         \n\
      TEMP temp1;                                         \n\
      TEX tex0, fragment.texcoord[0], texture[0], RECT;   \n\
      MUL temp0, color0, tex0;                            \n\
      TEX tex1, fragment.texcoord[1], texture[1], RECT;   \n\
      MUL temp1, color1, tex1;                            \n\
      MUL result.color, temp0, temp1;                     \n\
      END";

    m_Asm2TextureMod = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_Asm2TextureMod->LoadVertexShader(AsmVtx);
    m_Asm2TextureMod->LoadPixelShader(AsmFrg);
    m_Asm2TextureMod->Link();

    m_Asm2TextureRectMod = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_Asm2TextureRectMod->LoadVertexShader(AsmVtx);
    m_Asm2TextureRectMod->LoadPixelShader(AsmFrgRect);
    m_Asm2TextureRectMod->Link();
  }

  void GraphicsEngine::InitAsm4TextureAdd()
  {
    const char* AsmVtx = 
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
        END";

    const char* AsmFrg = 
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
        END";

    const char* AsmFrgRect = 
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
      END";

    m_Asm4TextureAdd = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_Asm4TextureAdd->LoadVertexShader(AsmVtx);
    m_Asm4TextureAdd->LoadPixelShader(AsmFrg);
    m_Asm4TextureAdd->Link();

    m_Asm4TextureRectAdd = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_Asm4TextureRectAdd->LoadVertexShader(AsmVtx);
    m_Asm4TextureRectAdd->LoadPixelShader(AsmFrgRect);
    m_Asm4TextureRectAdd->Link();
  }

  void GraphicsEngine::InitAsmBlendModes()
  {
    const char* AsmVtx = 
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
        END";

    const char* AsmPSBNormal = 
                             "!!ARBfp1.0                                         \n\
        TEMP tex0;                                          \n\
        TEMP tex1;                                          \n\
        TEX tex0, fragment.texcoord[0], texture[0], 2D;     \n\
        TEX tex1, fragment.texcoord[1], texture[1], 2D;     \n\
        MOV result.color, tex0;                             \n\
        END";

    m_AsmPSBNormal = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmPSBNormal->LoadVertexShader(AsmVtx);
    m_AsmPSBNormal->LoadPixelShader(AsmPSBNormal);
    m_AsmPSBNormal->Link();

    // Lighten
    const char* AsmPSBLighten = 
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
        END";

    m_AsmPSBLighten = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmPSBLighten->LoadVertexShader(AsmVtx);
    m_AsmPSBLighten->LoadPixelShader(AsmPSBLighten);
    m_AsmPSBLighten->Link();

    // Darken
    const char* AsmPSBDarken = 
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
        END";

    m_AsmPSBDarken = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmPSBDarken->LoadVertexShader(AsmVtx);
    m_AsmPSBDarken->LoadPixelShader(AsmPSBDarken);
    m_AsmPSBDarken->Link();

    // Multiply
    const char* AsmPSBMultiply = 
                               "!!ARBfp1.0                                         \n\
        TEMP tex0;                                          \n\
        TEMP tex1;                                          \n\
        TEMP temp;                                          \n\
        TEX tex0, fragment.texcoord[0], texture[0], 2D;     \n\
        TEX tex1, fragment.texcoord[1], texture[1], 2D;     \n\
        MUL result.color, tex0, tex1;                       \n\
        END";

    m_AsmPSBMultiply = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmPSBMultiply->LoadVertexShader(AsmVtx);
    m_AsmPSBMultiply->LoadPixelShader(AsmPSBMultiply);
    m_AsmPSBMultiply->Link();
  }



  void GraphicsEngine::QRP_ASM_Color(int x, int y, int width, int height, const Color &c0)
  {
    QRP_ASM_Color(x, y, width, height, c0, c0, c0, c0);
  }

  void GraphicsEngine::QRP_ASM_Color(int x, int y, int width, int height, const Color &c0, const Color &c1, const Color &c2, const Color &c3)
  {
    NUX_RETURN_IF_FALSE(m_AsmColor.IsValid());

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, c0.red, c0.green, c0.blue, c0.alpha,
      fx,          fy + height, 0.0f, 1.0f, c1.red, c1.green, c1.blue, c1.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, c2.red, c2.green, c2.blue, c2.alpha,
      fx + width,  fy,          0.0f, 1.0f, c3.red, c3.green, c3.blue, c3.alpha,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = m_AsmColor;

    shader_program->Begin();

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));


    int VertexLocation          = VTXATTRIB_POSITION;
    int VertexColorLocation     = VTXATTRIB_COLOR;

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    shader_program->End();
  }

  void GraphicsEngine::QRP_ASM_1Tex(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &color)
  {
    NUX_RETURN_IF_FALSE(m_AsmTextureModColor.IsValid());
    NUX_RETURN_IF_FALSE(m_AsmTextureRectModColor.IsValid());

    QRP_Compute_Texture_Coord(width, height, device_texture, texxform);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    
    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = m_AsmTextureModColor;
    if (device_texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = m_AsmTextureRectModColor;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, device_texture);

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));


    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int VertexColorLocation     = VTXATTRIB_COLOR;

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

    shader_program->End();
  }

  void GraphicsEngine::QRP_ASM_ColorModTexAlpha(int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &color)
  {
    NUX_RETURN_IF_FALSE(m_AsmColorModTexMaskAlpha.IsValid());
    NUX_RETURN_IF_FALSE(m_AsmColorModTexRectMaskAlpha.IsValid());

    QRP_Compute_Texture_Coord(width, height, device_texture, texxform);

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = m_AsmColorModTexMaskAlpha;
    if (device_texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = m_AsmColorModTexRectMaskAlpha;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, device_texture);

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));


    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int VertexColorLocation     = VTXATTRIB_COLOR;

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

    shader_program->End();
  }

  void GraphicsEngine::QRP_ASM_2Tex(int x, int y, int width, int height,
                                  ObjectPtr<IOpenGLBaseTexture> device_texture0, TexCoordXForm &texxform0, const Color &color0,
                                  ObjectPtr<IOpenGLBaseTexture> device_texture1, TexCoordXForm &texxform1, const Color &color1)
  {
    NUX_RETURN_IF_FALSE(m_Asm2TextureAdd.IsValid());
    NUX_RETURN_IF_FALSE(m_Asm2TextureRectAdd.IsValid());

    QRP_Compute_Texture_Coord(width, height, device_texture0, texxform0);
    QRP_Compute_Texture_Coord(width, height, device_texture1, texxform1);

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0.0f, 1.0f, texxform1.u0, texxform1.v0, 0.0f, 1.0f,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0.0f, 1.0f, texxform1.u0, texxform1.v1, 0.0f, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0.0f, 1.0f, texxform1.u1, texxform1.v1, 0.0f, 1.0f,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0.0f, 1.0f, texxform1.u1, texxform1.v0, 0.0f, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = m_Asm2TextureAdd;
    if (device_texture0->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = m_Asm2TextureRectAdd;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, device_texture0);
    SetTexture(GL_TEXTURE1, device_texture1);

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));

    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int TextureCoord1Location   = VTXATTRIB_TEXCOORD1;

    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, color0.red, color0.green, color0.blue, color0.alpha ));
    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, color1.red, color1.green, color1.blue, color1.alpha ));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord1Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (TextureCoord1Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord1Location));

    shader_program->End();
  }

  void GraphicsEngine::QRP_ASM_DisturbedTexture(
    int x, int y, int width, int height,
    ObjectPtr<IOpenGLBaseTexture> distorsion_texture, TexCoordXForm &texxform0, const Color& c0,
    ObjectPtr<IOpenGLBaseTexture> src_device_texture, TexCoordXForm &texxform1, const Color& c1)
  {
    NUX_RETURN_IF_FALSE(m_ASM2TextureDepRead.IsValid());
    NUX_RETURN_IF_FALSE(m_ASM2TextureRectDepRead.IsValid());

    QRP_Compute_Texture_Coord(width, height, distorsion_texture, texxform0);
    QRP_Compute_Texture_Coord(width, height, src_device_texture, texxform1);

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0.0f, 1.0f, texxform1.u0, texxform1.v0, 0.0f, 1.0f,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0.0f, 1.0f, texxform1.u0, texxform1.v1, 0.0f, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0.0f, 1.0f, texxform1.u1, texxform1.v1, 0.0f, 1.0f,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0.0f, 1.0f, texxform1.u1, texxform1.v0, 0.0f, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = m_ASM2TextureDepRead;
    if (src_device_texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = m_ASM2TextureRectDepRead;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, distorsion_texture);
    SetTexture(GL_TEXTURE1, src_device_texture);

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));

    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int TextureCoord1Location   = VTXATTRIB_TEXCOORD1;

    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, c0.red, c0.green, c0.blue, c0.alpha ));
    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, c1.red, c1.green, c1.blue, c1.alpha ));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord1Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (TextureCoord1Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord1Location));

    shader_program->End();
  }

  void GraphicsEngine::QRP_ASM_2TexMod(int x, int y, int width, int height,
    ObjectPtr<IOpenGLBaseTexture> device_texture0, TexCoordXForm &texxform0, const Color &color0,
    ObjectPtr<IOpenGLBaseTexture> device_texture1, TexCoordXForm &texxform1, const Color &color1)
  {
    NUX_RETURN_IF_FALSE(m_Asm2TextureMod.IsValid());
    NUX_RETURN_IF_FALSE(m_Asm2TextureRectMod.IsValid());

    QRP_Compute_Texture_Coord(width, height, device_texture0, texxform0);
    QRP_Compute_Texture_Coord(width, height, device_texture1, texxform1);

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0.0f, 1.0f, texxform1.u0, texxform1.v0, 0.0f, 1.0f,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0.0f, 1.0f, texxform1.u0, texxform1.v1, 0.0f, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0.0f, 1.0f, texxform1.u1, texxform1.v1, 0.0f, 1.0f,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0.0f, 1.0f, texxform1.u1, texxform1.v0, 0.0f, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = m_Asm2TextureMod;
    if (device_texture0->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = m_Asm2TextureRectMod;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, device_texture0);
    SetTexture(GL_TEXTURE1, device_texture1);

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));

    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int TextureCoord1Location   = VTXATTRIB_TEXCOORD1;

    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, color0.red, color0.green, color0.blue, color0.alpha ));
    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, color1.red, color1.green, color1.blue, color1.alpha ));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord1Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (TextureCoord1Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord1Location));

    shader_program->End();
  }

  void GraphicsEngine::QRP_ASM_4Tex(int x, int y, int width, int height,
                                  ObjectPtr<IOpenGLBaseTexture> device_texture0, TexCoordXForm &texxform0, const Color &color0,
                                  ObjectPtr<IOpenGLBaseTexture> device_texture1, TexCoordXForm &texxform1, const Color &color1,
                                  ObjectPtr<IOpenGLBaseTexture> device_texture2, TexCoordXForm &texxform2, const Color &color2,
                                  ObjectPtr<IOpenGLBaseTexture> device_texture3, TexCoordXForm &texxform3, const Color &color3)
  {
    NUX_RETURN_IF_FALSE(m_Asm4TextureAdd.IsValid());
    NUX_RETURN_IF_FALSE(m_Asm4TextureRectAdd.IsValid());

    QRP_Compute_Texture_Coord(width, height, device_texture0, texxform0);
    QRP_Compute_Texture_Coord(width, height, device_texture1, texxform1);
    QRP_Compute_Texture_Coord(width, height, device_texture2, texxform1);
    QRP_Compute_Texture_Coord(width, height, device_texture3, texxform1);

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 1.0f, texxform1.u0, texxform1.v0, 0, 1.0f, texxform2.u0, texxform2.v0, 0, 1.0f, texxform3.u0, texxform3.v0, 0, 1.0f,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 1.0f, texxform1.u0, texxform1.v1, 0, 1.0f, texxform2.u0, texxform2.v1, 0, 1.0f, texxform3.u0, texxform3.v1, 0, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 1.0f, texxform1.u1, texxform1.v1, 0, 1.0f, texxform2.u1, texxform2.v1, 0, 1.0f, texxform3.u1, texxform3.v1, 0, 1.0f,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 1.0f, texxform1.u1, texxform1.v0, 0, 1.0f, texxform2.u1, texxform2.v0, 0, 1.0f, texxform3.u1, texxform3.v0, 0, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = m_Asm4TextureAdd;
    if (device_texture0->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = m_Asm4TextureRectAdd;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, device_texture0);
    SetTexture(GL_TEXTURE1, device_texture1);
    SetTexture(GL_TEXTURE2, device_texture1);
    SetTexture(GL_TEXTURE3, device_texture1);

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));

    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int TextureCoord1Location   = VTXATTRIB_TEXCOORD1;
    int TextureCoord2Location   = VTXATTRIB_TEXCOORD2;
    int TextureCoord3Location   = VTXATTRIB_TEXCOORD3;

    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, color0.red, color0.green, color0.blue, color0.alpha ));
    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, color1.red, color1.green, color1.blue, color1.alpha ));
    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, color2.red, color2.green, color2.blue, color2.alpha ));
    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 3, color3.red, color3.green, color3.blue, color3.alpha ));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 4));
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord1Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 8));
    }

    if (TextureCoord2Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord2Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord2Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 12));
    }

    if (TextureCoord3Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord3Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord3Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 16));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (TextureCoord1Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord1Location));

    if (TextureCoord2Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord2Location));

    if (TextureCoord3Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord3Location));

    shader_program->End();
  }

////////////////////////////////////////////////////////////////////////////////////
  void GraphicsEngine::QRP_ASM_Triangle(int x0, int y0,
                                      int x1, int y1,
                                      int x2, int y2,
                                      Color c0)
  {
    QRP_ASM_Triangle(x0, y0, x1, y1, x2, y2, c0, c0, c0);
  }

  void GraphicsEngine::QRP_ASM_Triangle(int x0, int y0,
                                      int x1, int y1,
                                      int x2, int y2,
                                      Color c0, Color c1, Color c2)
  {
    NUX_RETURN_IF_FALSE(m_AsmColor.IsValid());

    float VtxBuffer[] =
    {
      static_cast<float>(x0), static_cast<float>(y0), 0.0f, 1.0f, c0.red, c0.green, c0.blue, c0.alpha,
      static_cast<float>(x1), static_cast<float>(y1), 0.0f, 1.0f, c1.red, c1.green, c1.blue, c1.alpha,
      static_cast<float>(x2), static_cast<float>(y2), 0.0f, 1.0f, c2.red, c2.green, c2.blue, c2.alpha,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> ShaderProg = m_AsmColor;

    ShaderProg->Begin();

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));

    int VertexLocation          = VTXATTRIB_POSITION;
    int VertexColorLocation     = VTXATTRIB_COLOR;

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));

    CHECKGL(glDrawArrays(GL_TRIANGLES, 0, 3));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));
    ShaderProg->End();

    m_triangle_stats++;
  }

//////////////////////
// DRAW LINES       //
//////////////////////
  void GraphicsEngine::QRP_ASM_Line(int x0, int y0,
                                  int x1, int y1, Color c0)
  {
    QRP_ASM_Line(x0, y0, x1, y1, c0, c0);
  }

  void GraphicsEngine::QRP_ASM_Line(int x0, int y0,
                                  int x1, int y1, Color c0, Color c1)
  {
    NUX_RETURN_IF_FALSE(m_AsmColor.IsValid());

    float VtxBuffer[] =
    {
      static_cast<float>(x0), static_cast<float>(y0), 0.0f, 1.0f, c0.red, c0.green, c0.blue, c0.alpha,
      static_cast<float>(x1), static_cast<float>(y1), 0.0f, 1.0f, c1.red, c1.green, c1.blue, c1.alpha,
    };

    ObjectPtr<IOpenGLAsmShaderProgram> ShaderProg = m_AsmColor;


    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));

    int VertexLocation          = VTXATTRIB_POSITION;
    int VertexColorLocation     = VTXATTRIB_COLOR;

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_LINES, 0, 2));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();

    m_line_stats++;
  }

  void GraphicsEngine::QRP_ASM_QuadWireframe(int x0, int y0, int width, int height,
      Color c0,
      Color c1,
      Color c2,
      Color c3)
  {
    NUX_RETURN_IF_FALSE(m_AsmColor.IsValid());

    float fx0 = x0, fy0 = y0;
    float VtxBuffer[] =
    {
      fx0, fy0,                             0.0f, 1.0f, c0.red, c0.green, c0.blue, c0.alpha,
      fx0, fy0 + height - 1,                0.0f, 1.0f, c1.red, c1.green, c1.blue, c1.alpha,
      fx0 + width - 1, fy0 + height - 1,    0.0f, 1.0f, c2.red, c2.green, c2.blue, c2.alpha,
      fx0 + width - 1, fy0,                 0.0f, 1.0f, c3.red, c3.green, c3.blue, c3.alpha,
      fx0, fy0,                             0.0f, 1.0f, c0.red, c0.green, c0.blue, c0.alpha,
    };

    ObjectPtr<IOpenGLAsmShaderProgram> ShaderProg = m_AsmColor;

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));

    int VertexLocation          = VTXATTRIB_POSITION;
    int VertexColorLocation     = VTXATTRIB_COLOR;

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_LINE_STRIP, 0, 5));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();

    m_line_stats++;
  }

  void GraphicsEngine::InitAsmPower()
  {
    const char* AsmVtx = 
      "!!ARBvp1.0                                 \n\
      ATTRIB iPos         = vertex.position;      \n\
      OUTPUT oPos         = result.position;      \n\
      OUTPUT oTexCoord0   = result.texcoord[0];   \n\
      # Transform the vertex to clip coordinates. \n\
      DP4   oPos.x, state.matrix.mvp.row[0], vertex.position;      \n\
      DP4   oPos.y, state.matrix.mvp.row[1], vertex.position;      \n\
      DP4   oPos.z, state.matrix.mvp.row[2], vertex.position;      \n\
      DP4   oPos.w, state.matrix.mvp.row[3], vertex.position;      \n\
      MOV   oTexCoord0, vertex.attrib[8];       \n\
      END";


    const char* AsmFrg = 
      "!!ARBfp1.0                                       \n\
      PARAM color0 = program.local[0];                  \n\
      PARAM exponent = program.local[1];                \n\
      TEMP tex0;                                        \n\
      TEMP final;                                       \n\
      TEX tex0, fragment.texcoord[0], texture[0], 2D;   \n\
      POW final.r, tex0.r, exponent.r;             \n\
      POW final.g, tex0.g, exponent.g;             \n\
      POW final.b, tex0.b, exponent.b;             \n\
      MOV final.a, tex0.a;                         \n\
      MUL result.color, color0, final;             \n\
      END";

    _asm_tex_component_exponentiation_prog = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    _asm_tex_component_exponentiation_prog->LoadVertexShader(AsmVtx);
    _asm_tex_component_exponentiation_prog->LoadPixelShader(AsmFrg);
    _asm_tex_component_exponentiation_prog->Link();
  }

  void GraphicsEngine::QRP_ASM_Power  (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &c0, Vector4 exponent)
  {
    NUX_RETURN_IF_FALSE(_asm_tex_component_exponentiation_prog.IsValid());
    NUX_RETURN_IF_FALSE(_asm_texrect_component_exponentiation_prog.IsValid());

    QRP_Compute_Texture_Coord(width, height, device_texture, texxform);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 1.0f,
      fx,          fy + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 1.0f,
      fx + width,  fy,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = _asm_tex_component_exponentiation_prog;
    if (device_texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = _asm_texrect_component_exponentiation_prog;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, device_texture);

    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, c0.red, c0.green, c0.blue, c0.alpha));
    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, exponent.x, exponent.y, exponent.z, exponent.w));

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));


    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    shader_program->End();
  }

  void GraphicsEngine::InitAsmAlphaReplicate()
  {
    const char* AsmVtx = 
      "!!ARBvp1.0                                 \n\
      ATTRIB iPos         = vertex.position;      \n\
      OUTPUT oPos         = result.position;      \n\
      OUTPUT oTexCoord0   = result.texcoord[0];   \n\
      # Transform the vertex to clip coordinates. \n\
      DP4   oPos.x, state.matrix.mvp.row[0], vertex.position;      \n\
      DP4   oPos.y, state.matrix.mvp.row[1], vertex.position;      \n\
      DP4   oPos.z, state.matrix.mvp.row[2], vertex.position;      \n\
      DP4   oPos.w, state.matrix.mvp.row[3], vertex.position;      \n\
      MOV   oTexCoord0, vertex.attrib[8];       \n\
      END";


    const char* AsmFrg = 
      "!!ARBfp1.0                                     \n\
      PARAM color0 = program.local[0];                \n\
      TEMP tex0;                                      \n\
      TEX tex0, fragment.texcoord[0], texture[0], 2D; \n\
      MUL result.color, color0, tex0.aaaa;            \n\
      END";

    _asm_tex_alpha_replicate_prog = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    _asm_tex_alpha_replicate_prog->LoadVertexShader(AsmVtx);
    _asm_tex_alpha_replicate_prog->LoadPixelShader(AsmFrg);
    _asm_tex_alpha_replicate_prog->Link();

  }

  void GraphicsEngine::QRP_ASM_AlphaReplicate(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color &c0)
  {
    NUX_RETURN_IF_FALSE(_asm_tex_alpha_replicate_prog.IsValid());
    NUX_RETURN_IF_FALSE(_asm_texrect_alpha_replicate_prog.IsValid());

    QRP_Compute_Texture_Coord(width, height, device_texture, texxform);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 1.0f,
      fx,          fy + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 1.0f,
      fx + width,  fy,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = _asm_tex_alpha_replicate_prog;
    if (device_texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = _asm_texrect_alpha_replicate_prog;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, device_texture);

    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, c0.red, c0.green, c0.blue, c0.alpha));

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));


    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    shader_program->End();
  }

  void GraphicsEngine::InitAsmColorMatrixFilter()
  {
    const char* AsmVtx = 
      "!!ARBvp1.0                                               \n\
      ATTRIB iPos         = vertex.position;                    \n\
      OUTPUT oPos         = result.position;                    \n\
      OUTPUT oTexCoord0   = result.texcoord[0];                 \n\
      # Transform the vertex to clip coordinates.               \n\
      DP4   oPos.x, state.matrix.mvp.row[0], vertex.position;   \n\
      DP4   oPos.y, state.matrix.mvp.row[1], vertex.position;   \n\
      DP4   oPos.z, state.matrix.mvp.row[2], vertex.position;   \n\
      DP4   oPos.w, state.matrix.mvp.row[3], vertex.position;   \n\
      MOV   oTexCoord0, vertex.attrib[8];                       \n\
      END";


    const char* AsmFrg = 
      "!!ARBfp1.0                                     \n\
      PARAM color0 = program.local[0];                \n\
      PARAM CM0 = program.local[1];                   \n\
      PARAM CM1 = program.local[2];                   \n\
      PARAM CM2 = program.local[3];                   \n\
      PARAM CM3 = program.local[4];                   \n\
      PARAM offset = program.local[5];                \n\
      TEMP tex0;                                      \n\
      TEMP final;                                     \n\
      TEX tex0, fragment.texcoord[0], texture[0], 2D; \n\
      MAD final.r, tex0, CM0, offset.r;               \n\
      MAD final.g, tex0, CM1, offset.g;               \n\
      MAD final.b, tex0, CM2, offset.b;               \n\
      MAD final.a, tex0, CM3, offset.a;               \n\
      MUL result.color, color0, final;                \n\
      END";

    const char* AsmFrgRect = 
      "!!ARBfp1.0                                     \n\
      PARAM color0 = program.local[0];                \n\
      PARAM CM0 = program.local[1];                   \n\
      PARAM CM1 = program.local[2];                   \n\
      PARAM CM2 = program.local[3];                   \n\
      PARAM CM3 = program.local[4];                   \n\
      PARAM offset = program.local[5];                \n\
      TEMP tex0;                                      \n\
      TEMP final;                                     \n\
      TEX tex0, fragment.texcoord[0], texture[0], 2D; \n\
      MAD final.r, tex0, CM0, offset.r;               \n\
      MAD final.g, tex0, CM1, offset.g;               \n\
      MAD final.b, tex0, CM2, offset.b;               \n\
      MAD final.a, tex0, CM3, offset.a;               \n\
      MUL result.color, color0, final;                \n\
      END";

    _asm_tex_color_matrix_filter_prog = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    _asm_tex_color_matrix_filter_prog->LoadVertexShader(AsmVtx);
    _asm_tex_color_matrix_filter_prog->LoadPixelShader(AsmFrg);
    _asm_tex_color_matrix_filter_prog->Link();

    _asm_texrect_color_matrix_filter_prog = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    _asm_texrect_color_matrix_filter_prog->LoadVertexShader(AsmVtx);
    _asm_texrect_color_matrix_filter_prog->LoadPixelShader(AsmFrgRect);
    _asm_texrect_color_matrix_filter_prog->Link();
  }

  void GraphicsEngine::QRP_ASM_ColorMatrix  (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color &c0, Matrix4 color_matrix, Vector4 offset)
  {
    NUX_RETURN_IF_FALSE(_asm_tex_color_matrix_filter_prog.IsValid());
    NUX_RETURN_IF_FALSE(_asm_texrect_color_matrix_filter_prog.IsValid());

    QRP_Compute_Texture_Coord(width, height, device_texture, texxform);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 1.0f,
      fx,          fy + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 1.0f,
      fx + width,  fy,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = _asm_tex_color_matrix_filter_prog;
    if (device_texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = _asm_texrect_color_matrix_filter_prog;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, device_texture);

    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, c0.red, c0.green, c0.blue, c0.alpha));
    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, color_matrix.m[0][0], color_matrix.m[0][1], color_matrix.m[0][2], color_matrix.m[0][3]));
    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, color_matrix.m[1][0], color_matrix.m[1][1], color_matrix.m[1][2], color_matrix.m[1][3]));
    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 3, color_matrix.m[2][0], color_matrix.m[2][1], color_matrix.m[2][2], color_matrix.m[2][3]));
    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 4, color_matrix.m[3][0], color_matrix.m[3][1], color_matrix.m[3][2], color_matrix.m[3][3]));
    CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 5, offset.x, offset.y, offset.z, offset.w));

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));


    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    shader_program->End();
  }

  void GraphicsEngine::InitAsmSeparableGaussFilter()
  {
    const char* AsmVtx = 
      "!!ARBvp1.0                                 \n\
      ATTRIB iPos         = vertex.position;      \n\
      OUTPUT oPos         = result.position;      \n\
      OUTPUT oTexCoord0   = result.texcoord[0];   \n\
      OUTPUT oTexCoord1   = result.texcoord[1];   \n\
      OUTPUT oTexCoord2   = result.texcoord[2];   \n\
      OUTPUT oTexCoord3   = result.texcoord[3];   \n\
      OUTPUT oTexCoord4   = result.texcoord[4];   \n\
      OUTPUT oTexCoord5   = result.texcoord[5];   \n\
      OUTPUT oTexCoord6   = result.texcoord[6];   \n\
      # Transform the vertex to clip coordinates. \n\
      DP4   oPos.x, state.matrix.mvp.row[0], vertex.position;      \n\
      DP4   oPos.y, state.matrix.mvp.row[1], vertex.position;      \n\
      DP4   oPos.z, state.matrix.mvp.row[2], vertex.position;      \n\
      DP4   oPos.w, state.matrix.mvp.row[3], vertex.position;      \n\
      MOV   oTexCoord0, vertex.attrib[8];         \n\
      MOV   oTexCoord1, vertex.attrib[9];         \n\
      MOV   oTexCoord2, vertex.attrib[10];        \n\
      MOV   oTexCoord3, vertex.attrib[11];        \n\
      MOV   oTexCoord4, vertex.attrib[12];        \n\
      MOV   oTexCoord5, vertex.attrib[13];        \n\
      MOV   oTexCoord6, vertex.attrib[14];        \n\
      END";


    const char* AsmFrg = 
      "!!ARBfp1.0                                                   \n\
      TEMP tex0;                                                    \n\
      TEMP final;                                                   \n\
      MOV final, {0, 0, 0, 0};                                      \n\
      TEX tex0, fragment.texcoord[0], texture[0], 2D;               \n\
      MAD final, tex0, program.local[0], final;                     \n\
      TEX tex0, fragment.texcoord[1], texture[0], 2D;               \n\
      MAD final, tex0, program.local[1], final;                     \n\
      TEX tex0, fragment.texcoord[2], texture[0], 2D;               \n\
      MAD final, tex0, program.local[2], final;                     \n\
      TEX tex0, fragment.texcoord[3], texture[0], 2D;               \n\
      MAD final, tex0, program.local[3], final;                     \n\
      TEX tex0, fragment.texcoord[4], texture[0], 2D;               \n\
      MAD final, tex0, program.local[4], final;                     \n\
      TEX tex0, fragment.texcoord[5], texture[0], 2D;               \n\
      MAD final, tex0, program.local[5], final;                     \n\
      TEX tex0, fragment.texcoord[6], texture[0], 2D;               \n\
      MAD final, tex0, program.local[6], final;                     \n\
      MOV result.color, final;                                      \n\
      END";


    const char* AsmFrgRect = 
      "!!ARBfp1.0                                                   \n\
      TEMP tex0;                                                    \n\
      TEMP final;                                                   \n\
      MOV final, {0, 0, 0, 0};                                      \n\
      TEX tex0, fragment.texcoord[0], texture[0], RECT;             \n\
      MAD final, tex0, program.local[0], final;                     \n\
      TEX tex0, fragment.texcoord[1], texture[0], RECT;             \n\
      MAD final, tex0, program.local[1], final;                     \n\
      TEX tex0, fragment.texcoord[2], texture[0], RECT;             \n\
      MAD final, tex0, program.local[2], final;                     \n\
      TEX tex0, fragment.texcoord[3], texture[0], RECT;             \n\
      MAD final, tex0, program.local[3], final;                     \n\
      TEX tex0, fragment.texcoord[4], texture[0], RECT;             \n\
      MAD final, tex0, program.local[4], final;                     \n\
      TEX tex0, fragment.texcoord[5], texture[0], RECT;             \n\
      MAD final, tex0, program.local[5], final;                     \n\
      TEX tex0, fragment.texcoord[6], texture[0], RECT;             \n\
      MAD final, tex0, program.local[6], final;                     \n\
      MOV result.color, final;                                      \n\
      END";

    _asm_tex_separable_gauss_filter_prog = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    _asm_tex_separable_gauss_filter_prog->LoadVertexShader(AsmVtx);
    _asm_tex_separable_gauss_filter_prog->LoadPixelShader(AsmFrg);
    _asm_tex_separable_gauss_filter_prog->Link();

    _asm_texrect_separable_gauss_filter_prog = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    _asm_texrect_separable_gauss_filter_prog->LoadVertexShader(AsmVtx);
    _asm_texrect_separable_gauss_filter_prog->LoadPixelShader(AsmFrgRect);
    _asm_texrect_separable_gauss_filter_prog->Link();
  }

  void GraphicsEngine::QRP_ASM_HorizontalGauss(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color & /* c0 */, float /* sigma */)
  {
    NUX_RETURN_IF_FALSE(_asm_tex_separable_gauss_filter_prog.IsValid());
    NUX_RETURN_IF_FALSE(_asm_texrect_separable_gauss_filter_prog.IsValid());

    QRP_Compute_Texture_Coord(width, height, device_texture, texxform);

    float delta = 1.0f / device_texture->GetWidth();

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, 
      texxform.u0 - 3.0f * delta, texxform.v0, 0, 1.0f,
      texxform.u0 - 2.0f * delta, texxform.v0, 0, 1.0f,
      texxform.u0 - 1.0f * delta, texxform.v0, 0, 1.0f,
      texxform.u0 + 0.0f * delta, texxform.v0, 0, 1.0f,
      texxform.u0 + 1.0f * delta, texxform.v0, 0, 1.0f,
      texxform.u0 + 2.0f * delta, texxform.v0, 0, 1.0f,
      texxform.u0 + 3.0f * delta, texxform.v0, 0, 1.0f,
      fx,          fy + height, 0.0f, 1.0f,
      texxform.u0 - 3.0f * delta, texxform.v1, 0, 1.0f,
      texxform.u0 - 2.0f * delta, texxform.v1, 0, 1.0f,
      texxform.u0 - 1.0f * delta, texxform.v1, 0, 1.0f,
      texxform.u0 + 0.0f * delta, texxform.v1, 0, 1.0f,
      texxform.u0 + 1.0f * delta, texxform.v1, 0, 1.0f,
      texxform.u0 + 2.0f * delta, texxform.v1, 0, 1.0f,
      texxform.u0 + 3.0f * delta, texxform.v1, 0, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f,
      texxform.u1 - 3.0f * delta, texxform.v1, 0, 1.0f,
      texxform.u1 - 2.0f * delta, texxform.v1, 0, 1.0f,
      texxform.u1 - 1.0f * delta, texxform.v1, 0, 1.0f,
      texxform.u1 + 0.0f * delta, texxform.v1, 0, 1.0f,
      texxform.u1 + 1.0f * delta, texxform.v1, 0, 1.0f,
      texxform.u1 + 2.0f * delta, texxform.v1, 0, 1.0f,
      texxform.u1 + 3.0f * delta, texxform.v1, 0, 1.0f,
      fx + width,  fy,          0.0f, 1.0f,
      texxform.u1 - 3.0f * delta, texxform.v0, 0, 1.0f,
      texxform.u1 - 2.0f * delta, texxform.v0, 0, 1.0f,
      texxform.u1 - 1.0f * delta, texxform.v0, 0, 1.0f,
      texxform.u1 + 0.0f * delta, texxform.v0, 0, 1.0f,
      texxform.u1 + 1.0f * delta, texxform.v0, 0, 1.0f,
      texxform.u1 + 2.0f * delta, texxform.v0, 0, 1.0f,
      texxform.u1 + 3.0f * delta, texxform.v0, 0, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = _asm_tex_separable_gauss_filter_prog;
    if (device_texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = _asm_texrect_separable_gauss_filter_prog;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, device_texture);

    // Set the Gaussian weights
    {
      float *W;
      GaussianWeights(&W, 1, 7);

      for (int i = 0; i < 7; i++)
      {
        CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, i, W[i], W[i], W[i], W[i]));
      }
      delete[] W;
    }

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));


    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int TextureCoord1Location   = VTXATTRIB_TEXCOORD1;
    int TextureCoord2Location   = VTXATTRIB_TEXCOORD2;
    int TextureCoord3Location   = VTXATTRIB_TEXCOORD3;
    int TextureCoord4Location   = VTXATTRIB_TEXCOORD4;
    int TextureCoord5Location   = VTXATTRIB_TEXCOORD5;
    int TextureCoord6Location   = VTXATTRIB_TEXCOORD6;

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer));

    //if(TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord1Location));
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord2Location));
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord3Location));
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord4Location));
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord5Location));
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord6Location));

      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 4));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 8));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord2Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 12));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord3Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 16));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord4Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 20));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord5Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 24));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord6Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 28));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));
    if (TextureCoord1Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord1Location));
    if (TextureCoord2Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord2Location));
    if (TextureCoord3Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord3Location));
    if (TextureCoord4Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord4Location));
    if (TextureCoord5Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord5Location));
    if (TextureCoord6Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord6Location));

    shader_program->End();
  }

  void GraphicsEngine::QRP_ASM_VerticalGauss(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color & /* c0 */, float /* sigma */)
  {
    NUX_RETURN_IF_FALSE(_asm_tex_separable_gauss_filter_prog.IsValid());
    NUX_RETURN_IF_FALSE(_asm_texrect_separable_gauss_filter_prog.IsValid());

    QRP_Compute_Texture_Coord(width, height, device_texture, texxform);

    float delta = 1.0f / device_texture->GetHeight();

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f,
      texxform.u0, texxform.v0 - 3.0f * delta, 0, 1.0f,
      texxform.u0, texxform.v0 - 2.0f * delta, 0, 1.0f,
      texxform.u0, texxform.v0 - 1.0f * delta, 0, 1.0f,
      texxform.u0, texxform.v0 + 0.0f * delta, 0, 1.0f,
      texxform.u0, texxform.v0 + 1.0f * delta, 0, 1.0f,
      texxform.u0, texxform.v0 + 2.0f * delta, 0, 1.0f,
      texxform.u0, texxform.v0 + 3.0f * delta, 0, 1.0f,
      fx,          fy + height, 0.0f, 1.0f,
      texxform.u0, texxform.v1 - 3.0f * delta, 0, 1.0f,
      texxform.u0, texxform.v1 - 2.0f * delta, 0, 1.0f,
      texxform.u0, texxform.v1 - 1.0f * delta, 0, 1.0f,
      texxform.u0, texxform.v1 + 0.0f * delta, 0, 1.0f,
      texxform.u0, texxform.v1 + 1.0f * delta, 0, 1.0f,
      texxform.u0, texxform.v1 + 2.0f * delta, 0, 1.0f,
      texxform.u0, texxform.v1 + 3.0f * delta, 0, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f,
      texxform.u1, texxform.v1 - 3.0f * delta, 0, 1.0f,
      texxform.u1, texxform.v1 - 2.0f * delta, 0, 1.0f,
      texxform.u1, texxform.v1 - 1.0f * delta, 0, 1.0f,
      texxform.u1, texxform.v1 + 0.0f * delta, 0, 1.0f,
      texxform.u1, texxform.v1 + 1.0f * delta, 0, 1.0f,
      texxform.u1, texxform.v1 + 2.0f * delta, 0, 1.0f,
      texxform.u1, texxform.v1 + 3.0f * delta, 0, 1.0f,
      fx + width,  fy,          0.0f, 1.0f,
      texxform.u1, texxform.v0 - 3.0f * delta, 0, 1.0f,
      texxform.u1, texxform.v0 - 2.0f * delta, 0, 1.0f,
      texxform.u1, texxform.v0 - 1.0f * delta, 0, 1.0f,
      texxform.u1, texxform.v0 + 0.0f * delta, 0, 1.0f,
      texxform.u1, texxform.v0 + 1.0f * delta, 0, 1.0f,
      texxform.u1, texxform.v0 + 2.0f * delta, 0, 1.0f,
      texxform.u1, texxform.v0 + 3.0f * delta, 0, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = _asm_tex_separable_gauss_filter_prog;
    if (device_texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = _asm_texrect_separable_gauss_filter_prog;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, device_texture);

    // Set the Gaussian weights
    {
      float *W;
      GaussianWeights(&W, 1, 7);

      for (int i = 0; i < 7; i++)
      {
        CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, i, W[i], W[i], W[i], W[i]));
      }
      delete[] W;
    }

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));


    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int TextureCoord1Location   = VTXATTRIB_TEXCOORD1;
    int TextureCoord2Location   = VTXATTRIB_TEXCOORD2;
    int TextureCoord3Location   = VTXATTRIB_TEXCOORD3;
    int TextureCoord4Location   = VTXATTRIB_TEXCOORD4;
    int TextureCoord5Location   = VTXATTRIB_TEXCOORD5;
    int TextureCoord6Location   = VTXATTRIB_TEXCOORD6;

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer));

    //if(TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord1Location));
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord2Location));
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord3Location));
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord4Location));
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord5Location));
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord6Location));

      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 4));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 8));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord2Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 12));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord3Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 16));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord4Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 20));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord5Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 24));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord6Location, 4, GL_FLOAT, GL_FALSE, 128, VtxBuffer + 28));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));
    if (TextureCoord1Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord1Location));
    if (TextureCoord2Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord2Location));
    if (TextureCoord3Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord3Location));
    if (TextureCoord4Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord4Location));
    if (TextureCoord5Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord5Location));
    if (TextureCoord6Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord6Location));

    shader_program->End();
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_ASM_GetBlurTexture(
    int x, int y,
    int buffer_width, int buffer_height,
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color& c0,
    float /* sigma */, int num_pass)
  {
    //     _offscreen_color_rt0.Release();
    //     _offscreen_color_rt1.Release();
    //     _offscreen_depth_rt0.Release();
    //     _offscreen_depth_rt1.Release();

    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    num_pass = Clamp<int> (num_pass, 1, 5);

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    _offscreen_color_rt0->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt0->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt1->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt1->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, buffer_width, buffer_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    QRP_ASM_1Tex(x, y, quad_width, quad_height, device_texture, texxform, color::White);

    TexCoordXForm texxform1;
    for (int i = 0; i < num_pass; i++)
    {
      SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt1, _offscreen_depth_rt1, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      QRP_ASM_HorizontalGauss(0, 0, buffer_width, buffer_height, _offscreen_color_rt0, texxform1, c0);

      SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      QRP_ASM_VerticalGauss(0, 0, buffer_width, buffer_height, _offscreen_color_rt1, texxform1, c0);
    }

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
    }

    SetOrthographicProjectionMatrix(previous_width, previous_height);

    return _offscreen_color_rt0;
  }

  void GraphicsEngine::QRP_ASM_GetBlurTextureFx(
      int x, int y, int buffer_width, int buffer_height,
      FxStructure *fx_structure, TexCoordXForm &texxform, const Color& color, float /* sigma */, int num_pass)
  {
    int quad_width = fx_structure->src_texture->GetWidth();
    int quad_height = fx_structure->src_texture->GetHeight();

    num_pass = Clamp<int> (num_pass, 1, 5);

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    fx_structure->src_texture->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    fx_structure->src_texture->SetFiltering(GL_NEAREST, GL_NEAREST);
    fx_structure->dst_texture->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    fx_structure->dst_texture->SetFiltering(GL_NEAREST, GL_NEAREST);
    fx_structure->temp_texture->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    fx_structure->temp_texture->SetFiltering(GL_NEAREST, GL_NEAREST);
        
    SetFrameBufferHelper(_offscreen_fbo, fx_structure->dst_texture, _offscreen_depth_rt0, buffer_width, buffer_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    QRP_ASM_1Tex(x, y, quad_width, quad_height, fx_structure->src_texture, texxform, color::White);

    TexCoordXForm texxform1;
    for (int i = 0; i < num_pass; i++)
    {
      SetFrameBufferHelper(_offscreen_fbo, fx_structure->temp_texture, _offscreen_depth_rt1, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      QRP_ASM_HorizontalGauss(0, 0, buffer_width, buffer_height, fx_structure->dst_texture, texxform1, color);

      SetFrameBufferHelper(_offscreen_fbo, fx_structure->dst_texture, _offscreen_depth_rt0, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      QRP_ASM_VerticalGauss(0, 0, buffer_width, buffer_height, fx_structure->temp_texture, texxform1, color);
    }

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
    }
  }      

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_ASM_GetPower(
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color & c0, const Vector4 &exponent)
  {
    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    _offscreen_color_rt0->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt0->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt1->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt1->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, quad_width, quad_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_ASM_1Tex(0, 0, quad_width, quad_height, device_texture, texxform, color::White);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt1, _offscreen_depth_rt1, quad_width, quad_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_ASM_Power(0, 0, quad_width, quad_height, _offscreen_color_rt0, texxform, c0, exponent);

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
    }

    return _offscreen_color_rt1;
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_ASM_GetAlphaTexture(
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color & c0)
  {
    //     _offscreen_color_rt0.Release();
    //     _offscreen_color_rt1.Release();
    //     _offscreen_depth_rt0.Release();
    //     _offscreen_depth_rt1.Release();

    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    _offscreen_color_rt0->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt0->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt1->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt1->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, quad_width, quad_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_ASM_1Tex(0, 0, quad_width, quad_height, device_texture, texxform, color::White);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt1, _offscreen_depth_rt1, quad_width, quad_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_ASM_AlphaReplicate(0, 0, quad_width, quad_height, _offscreen_color_rt0, texxform, c0);

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
    }

    return _offscreen_color_rt1;
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_ASM_GetColorMatrixTexture(
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color & c0, Matrix4 color_matrix, Vector4 offset)
  {
    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    _offscreen_color_rt0->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt0->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt1->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt1->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, quad_width, quad_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_ASM_1Tex(0, 0, quad_width, quad_height, device_texture, texxform, color::White);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt1, _offscreen_depth_rt1, quad_width, quad_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_ASM_ColorMatrix(0, 0, quad_width, quad_height, _offscreen_color_rt0, texxform, c0, color_matrix, offset);

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
    }

    return _offscreen_color_rt1;
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_ASM_GetLQBlur(
    int /* x */, int /* y */,
    int /* buffer_width */, int /* buffer_height */,
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color& /* c0 */)
  {
    //     _offscreen_color_rt0.Release();
    //     _offscreen_color_rt1.Release();
    //     _offscreen_depth_rt0.Release();
    //     _offscreen_depth_rt1.Release();

    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width  = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width  = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    _offscreen_color_rt0->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt0->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt1->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt1->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt2->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt2->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt3->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt3->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, quad_width/2, quad_height/2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_ASM_1Tex(0, 0, quad_width / 2, quad_height / 2, device_texture, texxform, color::White);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt1, _offscreen_depth_rt1, quad_width/4, quad_height/4);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_ASM_1Tex(0, 0, quad_width / 4, quad_height / 4, _offscreen_color_rt0, texxform, color::White);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt2, _offscreen_depth_rt2, quad_width/8, quad_height/8);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_ASM_1Tex(0, 0, quad_width / 8, quad_height / 8, _offscreen_color_rt1, texxform, color::White);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt3, _offscreen_depth_rt3, quad_width, quad_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    TexCoordXForm texxform0;
    TexCoordXForm texxform1;
    TexCoordXForm texxform2;
    TexCoordXForm texxform3;

    texxform0.flip_v_coord = true;
    texxform2.flip_v_coord = true;
    QRP_ASM_4Tex(0, 0, quad_width, quad_height,
      device_texture, texxform0, Color(0.25, 0.25, 0.25, 0.25),
      _offscreen_color_rt0, texxform1, Color(0.25, 0.25, 0.25, 0.25),
      _offscreen_color_rt1, texxform2, Color(0.25, 0.25, 0.25, 0.25),
      _offscreen_color_rt2, texxform3, Color(0.25, 0.25, 0.25, 0.25));

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
    }
    return _offscreen_color_rt3;
  }

  void GraphicsEngine::InitAsmPixelateShader()
  {
    const char* AsmVtx = 
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
      END";

    const char* AsmFrg = 
      "!!ARBfp1.0                                       \n\
      TEMP tex0;                                        \n\
      TEMP tex_coord;                                   \n\
      PARAM pixel_size = program.local [0];             \n\
      PARAM pixel_size_inv = program.local [1];         \n\
      MUL tex_coord, fragment.texcoord[0], pixel_size_inv; \n\
      FLR tex_coord, tex_coord;                         \n\
      MUL tex_coord, tex_coord, pixel_size;  \n\
      TEX tex0, tex_coord, texture[0], 2D;              \n\
      MUL result.color, fragment.color, tex0;           \n\
      END";

    const char* AsmFrgRect = 
      "!!ARBfp1.0                                       \n\
      TEMP tex0;                                        \n\
      TEMP tex_coord;                                   \n\
      PARAM pixel_size = program.local [0];             \n\
      PARAM pixel_size_inv = program.local [1];         \n\
      MUL tex_coord, fragment.texcoord[0], pixel_size_inv; \n\
      FLR tex_coord, tex_coord;                         \n\
      MUL tex_coord, tex_coord, pixel_size;  \n\
      TEX tex0, tex_coord, texture[0], RECT;            \n\
      MUL result.color, fragment.color, tex0;           \n\
      END";

    m_AsmPixelate = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmPixelate->LoadVertexShader(AsmVtx);
    m_AsmPixelate->LoadPixelShader(AsmFrg);
    m_AsmPixelate->Link();

    m_AsmPixelateRect = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmPixelateRect->LoadVertexShader(AsmVtx);
    m_AsmPixelateRect->LoadPixelShader(AsmFrgRect);
    m_AsmPixelateRect->Link();
  }

  void GraphicsEngine::QRP_ASM_Pixelate(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &color, int pixel_size)
  {
    NUX_RETURN_IF_FALSE(m_AsmPixelate.IsValid());
    NUX_RETURN_IF_FALSE(m_AsmPixelateRect.IsValid());

    if (pixel_size <= 0)
      pixel_size = 1;

    QRP_Compute_Texture_Coord(width, height, device_texture, texxform);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
    };

    float tex_width = device_texture->GetWidth();
    float tex_height = device_texture->GetHeight();

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    bool rectangle_texture = false;
    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = m_AsmPixelate;
    if (device_texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = m_AsmPixelateRect;
      rectangle_texture = true;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, device_texture);

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));

    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int VertexColorLocation     = VTXATTRIB_COLOR;

    if (rectangle_texture == false)
    {
      CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, (float)pixel_size/(float)tex_width, (float)pixel_size/(float)tex_height, 1.0f, 1.0f));
      CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, (float)tex_width/(float)pixel_size, (float)tex_height/(float)pixel_size, 1.0f, 1.0f));
    }
    else
    {
      CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, pixel_size, pixel_size, 1.0f, 1.0f));
      CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, 1.0f/pixel_size, 1.0f/pixel_size, 1.0f, 1.0f));
    }

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

    shader_program->End();
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_ASM_GetPixelBlocks(
	  ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform0, const Color& c0, int pixel_size)
  {
	  int quad_width = device_texture->GetWidth();
	  int quad_height = device_texture->GetHeight();

	  ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
	  int previous_width = 0;
	  int previous_height = 0;
	  if (prevFBO.IsValid())
	  {
		  previous_width  = prevFBO->GetWidth();
		  previous_height = prevFBO->GetHeight();
	  }
	  else
	  {
		  previous_width  = _graphics_display.GetWindowWidth();
		  previous_height = _graphics_display.GetWindowHeight();
	  }

    CHECKGL(glClearColor(0, 0, 0, 0));
	  SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, quad_width, quad_height);
	  CHECKGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
	  QRP_ASM_Pixelate(0, 0, quad_width, quad_height, device_texture, texxform0, c0, pixel_size);

	  _offscreen_fbo->Deactivate();

	  if (prevFBO.IsValid())
	  {
		  prevFBO->Activate(true);
		  SetViewport(0, 0, previous_width, previous_height);
	  }
	  else
	  {
		  SetViewport(0, 0, previous_width, previous_height);
	  }
	  return _offscreen_color_rt0;
  }

  void GraphicsEngine::QRP_ASM_GetCopyTexture(
    int width, int height,
    ObjectPtr<IOpenGLBaseTexture>& dst_device_texture,
    ObjectPtr<IOpenGLBaseTexture>& src_device_texture,
    TexCoordXForm &texxform0, const Color& c0)
  {
    if (src_device_texture.IsValid() == false)
    {
      return;
    }

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;

    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    if ((dst_device_texture.IsValid() == false) ||
      (dst_device_texture->GetWidth() != width) ||
      (dst_device_texture->GetHeight() != height) ||
      (dst_device_texture->GetPixelFormat() != src_device_texture->GetPixelFormat()))
    {
      dst_device_texture = _graphics_display.GetGpuDevice()->CreateTexture(width, height, 1, src_device_texture->GetPixelFormat());
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    SetFrameBufferHelper(_offscreen_fbo, dst_device_texture, _offscreen_depth_rt0, width, height);
    CHECKGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    QRP_ASM_1Tex(0, 0, width, height, src_device_texture, texxform0, c0);

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
    }
  }

  void GraphicsEngine::InitAsmTexturePremultiplyShader()
  {
    std::string AsmVtx = 
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
      END";

    std::string AsmFrg = 
      "!!ARBfp1.0                                       \n\
      TEMP tex0;                                        \n\
      TEMP temp;                                        \n\
      TEX tex0, fragment.texcoord[0], texture[0], 2D;   \n\
      MUL temp.rgb, tex0, tex0.aaaa;                    \n\
      MOV temp.a, tex0.aaaa;                            \n\
      MUL result.color, fragment.color, temp;           \n\
      END";

    std::string AsmFrgRect = 
      "!!ARBfp1.0                                       \n\
      TEMP tex0;                                        \n\
      TEMP temp;                                        \n\
      TEX tex0, fragment.texcoord[0], texture[0], RECT; \n\
      MUL temp.rgb, tex0, tex0.aaaa;                    \n\
      MOV temp.a, tex0.aaaa;                            \n\
      MUL result.color, fragment.color, temp;           \n\
      END";

    m_AsmTexturePremultiplyModColor = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmTexturePremultiplyModColor->LoadVertexShader(AsmVtx.c_str());
    m_AsmTexturePremultiplyModColor->LoadPixelShader(AsmFrg.c_str());
    m_AsmTexturePremultiplyModColor->Link();

    m_AsmTexturePremultiplyRectModColor = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
    m_AsmTexturePremultiplyRectModColor->LoadVertexShader(AsmVtx.c_str());
    m_AsmTexturePremultiplyRectModColor->LoadPixelShader(AsmFrgRect.c_str());
    m_AsmTexturePremultiplyRectModColor->Link();
  }

  void GraphicsEngine::QRP_ASM_1TexPremultiply(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &color)
  {
    if (device_texture.IsNull())
      return;

    NUX_RETURN_IF_FALSE(m_AsmTexturePremultiplyModColor.IsValid());
    NUX_RETURN_IF_FALSE(m_AsmTexturePremultiplyRectModColor.IsValid());

    QRP_Compute_Texture_Coord(width, height, device_texture, texxform);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 1.0f, color.red, color.green, color.blue, color.alpha,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program = m_AsmTexturePremultiplyModColor;
    if (device_texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
    {
      shader_program = m_AsmTexturePremultiplyRectModColor;
    }
    shader_program->Begin();

    SetTexture(GL_TEXTURE0, device_texture);

    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLModelViewMatrix().m));
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glLoadMatrixf((FLOAT *) GetOpenGLProjectionMatrix().m));


    int VertexLocation          = VTXATTRIB_POSITION;
    int TextureCoord0Location   = VTXATTRIB_TEXCOORD0;
    int VertexColorLocation     = VTXATTRIB_COLOR;

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

    shader_program->End();
  }
}
#endif // NUX_OPENGLES_20

