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


#ifndef NMESHOBJECT_H
#define NMESHOBJECT_H

#include "NuxCore/NKernel.h"
#include "NuxCore/Math/Vector2.h"
#include "NuxCore/Math/Vector3.h"
#include "NuxCore/Math/Vector4.h"

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

#include "NWorldObject.h"
#include "SceneData.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

  class Float3Set
  {
  public:
    Float3Set (int N)
    {
      Num = N;
      Pool = new nux::Vec3<float>[Num];
    }
    ~Float3Set()
    {
      NUX_SAFE_DELETE (Pool);
    }
    nux::Vec3<float>* Pool;
    int Num;
  };

  class Float4Set
  {
  public:
    Float4Set (int N)
    {
      Num = N;
      Pool = new nux::Vec4<float>[Num];
    }
    ~Float4Set()
    {
      NUX_SAFE_DELETE (Pool);
    }
    nux::Vec4<float>* Pool;
    int Num;
  };

  class Float2Set
  {
  public:
    Float2Set (int N)
    {
      Num = N;
      Pool = new nux::Vec2<float>[Num];
    }
    ~Float2Set()
    {
      NUX_SAFE_DELETE (Pool);
    }
    nux::Vec2<float>* Pool;
    int Num;
  };

  class AttributeIndexSet
  {
  public:
    AttributeIndexSet (int N)
    {
      Num = N;
      IndexPool = new Index3[Num];
    }
    ~AttributeIndexSet()
    {
      NUX_SAFE_DELETE (IndexPool);
    }
    Index3 *IndexPool;
    int Num;
  };

  class NMeshData
  {
  public:
    NMeshData (int NumTriangles);
    ~NMeshData();

    int GetNumTriangles() const
    {
      return m_NumTriangles;
    }

    void SetMatrix (nux::Matrix4 matrix)
    {
      m_Matrix = matrix;
    }
    nux::Matrix4 &GetMatrix()
    {
      return m_Matrix;
    }
    const nux::Matrix4 &GetMatrix() const
    {
      return m_Matrix;
    }

  public:
    nux::Matrix4 m_Matrix;
    std::vector<Float4Set *> VertexSetArray;     // There should be only one element in this array
    std::vector<Float2Set *> TextureSetArray;   // There can be 0 or more elements in this array. All the elements have the TEXTURE semantic.
    std::vector<Float4Set *> ColorSetArray;       // There can be 0 or more elements in this array. All the elements have the COLOR semantic.
    std::vector<Float3Set *> NormalSetArray;     // There can be 0 or more elements in this array. All the elements have the NORMAL semantic.
    std::vector<Float4Set *> TangentSetArray;    // There can be 0 or 1 elements in this array. The element have the TANGENT semantic.
    std::vector<Float4Set *> BinormalSetArray;   // There can be 0 or 1 elements in this array. The element have the BINORMAL semantic.

    std::vector<AttributeIndexSet *> VertexSetIndexArray;
    std::vector<AttributeIndexSet *> NormalSetIndexArray;
    std::vector<AttributeIndexSet *> ColorSetIndexArray;
    std::vector<AttributeIndexSet *> TextureSetIndexArray;
    std::vector<AttributeIndexSet *> TangentSetIndexArray;
    std::vector<AttributeIndexSet *> BinormalSetIndexArray;

  private:
    int m_NumTriangles;
  };

  class NMeshObject: public NWorldObject
  {
  public:
    NMeshObject (NMeshData *meshdata = 0);
    ~NMeshObject();

    void CreateVertexBuffer (NMeshData *meshdata);
    void CalculateTangentArray (const nux::Vec4<float> *vertex,
                                const nux::Vec3<float> *normal,
                                const nux::Vec2<float> *texcoord,
                                long triangleCount,
                                const unsigned int *triangleIndex,
                                nux::Vec3<float> *tangent,
                                nux::Vec3<float> *binormal);

    void CreateGLObjects();
    void DrawGLObjects();
    void RenderNormal();

    unsigned int GetVertexBufferSize();
    unsigned int GetNormalBufferSize();
    unsigned int GetColorBufferSize();
    unsigned int GetTextureBufferSize();
    unsigned int GetIndexBufferSize();
    unsigned int GetNumVertex();
    unsigned int GetNumIndex(); // The number of index used to draw may be more or less than the number of vertices. Depending on what you want to do...
    unsigned int GetMinIndex() const
    {
      return m_MinIndex;
    }
    unsigned int GetMaxIndex() const
    {
      return m_MaxIndex;
    }
    unsigned int GetNumPolygon() const
    {
      return m_NumPolygon;
    }
    bool HasNormal() const
    {
      return NumNormalAttributes > 0;
    }
    bool HasTangent() const
    {
      return NumTangentAttributes > 0;
    }
    bool HasBinormal() const
    {
      return NumBinormalAttributes > 0;
    }
    bool HasColor() const
    {
      return NumColorAttributes > 0;
    }
    bool HasTexCoord() const
    {
      return NumTextureAttributes > 0;
    }
    bool HasTangentSpace() const
    {
      return m_hasTangentSpace;
    }

    nux::Matrix4 &GetMatrix();
    const nux::Matrix4 &GetMatrix() const;

    std::vector<unsigned int> m_IndexArray;
    nux::Vec4<float>* m_VertexArray;
    nux::Vec4<float>** m_ColorArray;  // <--- this should be RGBA
    nux::Vec3<float>** m_NormalArray;
    nux::Vec3<float>** m_TangentArrayXSI;
    nux::Vec3<float>** m_BinormalArrayXSI;
    nux::Vec2<float>** m_TextureArray;

    // Computed with just the normal and the texture coordinates
    // See http://www.terathon.com/code/tangent.html
    nux::Vec3<float>* m_TangentArrayCustom;
    nux::Vec3<float>* m_BinormalArrayCustom;

    GLuint GetOpenGLId (int index = 0) const
    {
      return m_OpenGLID[index];
    }
    GLuint m_OpenGLID[16];  // index 0 is reserved for vertex position.
    GLuint m_GLVBOIndexVertex;
    GLuint m_GLVBOBiNormalCustom;
    GLuint m_GLVBOTangentCustom;

    // Stats
    int m_MinIndex;
    int m_MaxIndex;

  public:
    NMeshData *m_MeshData;
    int m_NumPolygon;

    int NumColorAttributes;
    int NumTextureAttributes;
    int NumNormalAttributes;
    int NumTangentAttributes;
    int NumBinormalAttributes;

    int m_NormalIndex;
    int m_TangentIndex;
    int m_BinormalIndex;
    int m_Color0Index;
    int m_TexCoordIndex;

    bool m_hasTangentSpace;
    int m_TexCoordCount;

    nux::Matrix4 m_WorldMatrix;

  };
} //NUX_NAMESPACE_END

#endif // NMESHOBJECT_H
