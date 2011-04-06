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
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef GLRENDERABLEMESH_H
#define GLRENDERABLEMESH_H

#include "NuxCore/Math/Vector3.h"
#include "NuxCore/Math/Matrix4.h"

#if defined(NUX_OS_WINDOWS)
#include "GL/glew.h"
#include "GL/wglew.h"
#elif defined(NUX_OS_LINUX)
#define GLEW_MX
#include "GL/glew.h"
#include "GL/glxew.h"
#else
#error Fixme
#endif

#include "SceneData.h"

class GLRenderableMesh
{
public:
  GLRenderableMesh();
  ~GLRenderableMesh();

  void LoadXSI (const TCHAR *filename);

  void Unload();

  //bool Initialize(MeshData2* MeshData);
  bool CreateGLArrayData (MeshData2 *MeshData);
  void CreateGLArrayData3 (MeshData3 *MeshData);

  int GetVertexBufferSize (int submeshid = 0);
  int GetNormalBufferSize (int submeshid = 0);
  int GetColorBufferSize (int submeshid = 0);
  int GetTextureBufferSize (int submeshid = 0);
  int GetIndexBufferSize (int submeshid = 0);

  int GetElementCount (int submeshid = 0);

  void LoadVBO();

  void DrawVBO();
  //void renderVertexAttributes(GLProgramObject* shaderProgram /*unsigned int shader_prg*/);
  int setVertexAttribute (unsigned int shader_program, int attribute_index, const char *attribute_name);

  void RenderNormal();

  unsigned int GetVertexCount (int List) const
  {
    return 0 /*m_SubMeshCount[i]*3*/;
  };
  bool HasNormals() const
  {
    return m_hasNormals;
  };
  bool HasColors() const
  {
    return m_hasColors;
  };
  bool HasTexCoord() const
  {
    return m_hasTexCoords;
  };
  bool HasTangentSpace() const
  {
    return m_hasTangentSpace;
  };

  nux::Matrix4 getLocalToWorldMatrix();

  void CalculateTangentArray (const nux::Vec4<float> *vertex,
                              const nux::Vec3<float> *normal,
                              const nux::Vec2<float> *texcoord,
                              long triangleCount,
                              const int *triangleIndex,
                              nux::Vec3<float> *tangent,
                              nux::Vec3<float> *binormal);

  nux::Vec4<float>** m_VertexArray;
  nux::Vec4<float>** m_ColorArray;  // <--- this should be RGBA
  nux::Vec3<float>** m_NormalArray;
  nux::Vec3<float>** m_TangentArray;
  nux::Vec3<float>** m_BinormalArray;
  nux::Vec2<float>* **m_UVArray;
  int **m_IndexArray;
  typedef struct
  {
    int MinIndex;
    int MaxIndex;
    int nbTriangles;
  } ArrayStats;

  ArrayStats *m_ArrayStats;



  GLuint m_GLVBOVertex;
  GLuint m_GLVBOColor;
  GLuint m_GLVBONormal;
  GLuint m_GLVBOBiNormal;
  GLuint m_GLVBOTangent;

  GLuint m_GLVBOTexture0;
  GLuint m_GLVBOTexture1;
  GLuint m_GLVBOTexture2;
  GLuint m_GLVBOTexture3;
  GLuint m_GLVBOTexture4;
  GLuint m_GLVBOTexture5;
  GLuint m_GLVBOTexture6;
  GLuint m_GLVBOTexture7;
  GLuint m_GLVBOTexture8;
  GLuint m_GLVBOTexture9;
  GLuint m_GLVBOTexture10;
  GLuint m_GLVBOTexture11;
  GLuint m_GLVBOTexture12;
  GLuint m_GLVBOTexture13;
  GLuint m_GLVBOTexture14;
  GLuint m_GLVBOTexture15;

  GLuint m_GLVBOIndexVertex;

private:
  int *m_SubMeshTriangleCount;
  unsigned int m_SubMeshCount;
  //unsigned int m_TriangleCount;

  bool m_hasNormals;
  bool m_hasColors;
  bool m_hasTexCoords;
  bool m_hasTangentSpace;
  int m_TexCoordCount;


  nux::Vector3 m_scaling;
  nux::Vector3 m_rotation;
  nux::Vector3 m_translation;


};

#endif // GLRENDERABLEMESH_H
