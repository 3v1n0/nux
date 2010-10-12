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


#include "NuxCore/NuxCore.h"
#include "NMeshObject.h"


#define CHECKGLX(a) a

#if defined(NUX_OS_WINDOWS)
extern GLEWContext *glewGetContext();
extern WGLEWContext *wglewGetContext();
#elif defined(NUX_OS_LINUX)
extern GLEWContext *glewGetContext();
extern GLXEWContext *glxewGetContext();
#endif

namespace nux
{

  NMeshData::NMeshData (int NumTriangles)
    :   m_NumTriangles (NumTriangles)
  {

  }

  NMeshData::~NMeshData()
  {

  }

  NMeshObject::NMeshObject (NMeshData *meshdata)
  {
    m_VertexArray           = 0;
    m_ColorArray            = 0;
    m_NormalArray           = 0;
    m_TangentArrayXSI       = 0;
    m_BinormalArrayXSI      = 0;
    m_TextureArray          = 0;

    m_TangentArrayCustom    = 0;
    m_BinormalArrayCustom   = 0;

    m_GLVBOIndexVertex      = 0;

    // Min Max index
    m_MinIndex = 0;
    m_MaxIndex = 0;

    m_NormalIndex       = 0;
    m_TangentIndex      = 0;
    m_BinormalIndex     = 0;
    m_Color0Index       = 0;
    m_TexCoordIndex     = 0;

    m_NumPolygon            = 0;

    NumColorAttributes      = 0;
    NumTextureAttributes    = 0;
    NumNormalAttributes     = 0;
    NumTangentAttributes    = 0;
    NumBinormalAttributes   = 0;
    m_hasTangentSpace       = false;

    m_TexCoordCount         = 0;

    m_WorldMatrix = nux::Matrix4::IDENTITY();
    nux::Memset (&m_OpenGLID, 0, sizeof (GLuint) * 16);

    if (meshdata)
    {
      CreateVertexBuffer (meshdata);
      CreateGLObjects();
    }
  }

  NMeshObject::~NMeshObject()
  {
    if (m_VertexArray)
    {
      for (long i = 0; i < NumColorAttributes; i++)
      {
        delete [] m_ColorArray[i];
      }

      for (long i = 0; i < NumNormalAttributes; i++)
      {
        delete [] m_NormalArray[i];
      }

      for (long i = 0; i < NumTextureAttributes; i++)
      {
        delete [] m_TextureArray[i];
      }

      delete [] m_VertexArray;
      delete [] m_ColorArray;
      delete [] m_NormalArray;
      delete [] m_TextureArray;
      delete [] m_TangentArrayCustom;
      delete [] m_BinormalArrayCustom;
    }
  }

  void NMeshObject::CreateVertexBuffer (NMeshData *meshdata)
  {
    if (!meshdata)
      return;

    NMeshData *md = meshdata;

    m_WorldMatrix = meshdata->GetMatrix();

    m_NumPolygon = 0;

    m_hasTangentSpace = false;

    {
      int nbTriangle = md->GetNumTriangles();
      m_NumPolygon = nbTriangle;
      m_VertexArray = new nux::Vec4<float> [nbTriangle*3];

      ////// Vertex ////////
      int count = 0;
      nux::Vec4<float> v0, v1, v2;

      for (long j = 0; j < nbTriangle; j++)
      {
        v0 = md->VertexSetArray[0]->Pool[md->VertexSetIndexArray[0]->IndexPool[j].i];
        v1 = md->VertexSetArray[0]->Pool[md->VertexSetIndexArray[0]->IndexPool[j].j];
        v2 = md->VertexSetArray[0]->Pool[md->VertexSetIndexArray[0]->IndexPool[j].k];

        //            v0.x *= 0.001; v0.y *= 0.001; v0.z *= 0.001;
        //            v1.x *= 0.001; v1.y *= 0.001; v1.z *= 0.001;
        //            v2.x *= 0.001; v2.y *= 0.001; v2.z *= 0.001;
        m_VertexArray[count++] = v0;
        m_VertexArray[count++] = v1;
        m_VertexArray[count++] = v2;
      }

      m_IndexArray.resize (nbTriangle * 3);

      ////// Index ////////
      // After that the VertexArray for mesh #Mesh is like:
      // [Triangle0<v0,v1,v2>, Triangle1<v0,v1,v2> .... TriangleN<v0,v1,v2>]
      // The index array is:
      // [Triangle0<0,1,2>, Triangle1<3,4,5>, ..... TriangleN<3N,3N+1,3N+2>]
      int nbIndex = nbTriangle * 3;

      m_MaxIndex = 0;
      m_MinIndex = 0xFFFFFFFF;

      for (int index = 0; index < nbIndex; index++)
      {
        m_IndexArray[index] = index;

        if (m_MaxIndex < index)
        {
          m_MaxIndex = index;
        }

        if (m_MinIndex > index)
        {
          m_MinIndex = index;
        }
      }

      ////// Color ////////
      NumColorAttributes = md->ColorSetArray.size();
      m_ColorArray = new nux::Vec4<float>* [NumColorAttributes];

      for (int ncolor = 0; ncolor < NumColorAttributes; ncolor++)
      {
        m_ColorArray[ncolor] = new nux::Vec4<float> [nbTriangle*3];

        int count = 0;
        nux::Vec4<float> v0, v1, v2;

        for (long j = 0; j < nbTriangle; j++)
        {
          v0 = md->ColorSetArray[ncolor]->Pool[md->ColorSetIndexArray[ncolor]->IndexPool[j].i];
          v1 = md->ColorSetArray[ncolor]->Pool[md->ColorSetIndexArray[ncolor]->IndexPool[j].j];
          v2 = md->ColorSetArray[ncolor]->Pool[md->ColorSetIndexArray[ncolor]->IndexPool[j].k];

          m_ColorArray[ncolor][count++] = v0;
          m_ColorArray[ncolor][count++] = v1;
          m_ColorArray[ncolor][count++] = v2;
        }
      }

      ////// Normal ////////
      NumNormalAttributes = md->NormalSetArray.size();
      m_NormalArray = new nux::Vec3<float>* [NumNormalAttributes];

      for (int nnormal = 0; nnormal < NumNormalAttributes; nnormal++)
      {
        m_NormalArray[nnormal] = new nux::Vec3<float> [nbTriangle*3];

        int count = 0;
        nux::Vec3<float> v0, v1, v2;

        for (long j = 0; j < nbTriangle; j++)
        {
          md->NormalSetIndexArray[nnormal]->IndexPool[j].i;
          v0 = md->NormalSetArray[nnormal]->Pool[md->NormalSetIndexArray[nnormal]->IndexPool[j].i];
          v1 = md->NormalSetArray[nnormal]->Pool[md->NormalSetIndexArray[nnormal]->IndexPool[j].j];
          v2 = md->NormalSetArray[nnormal]->Pool[md->NormalSetIndexArray[nnormal]->IndexPool[j].k];

          m_NormalArray[nnormal][count++] = v0;
          m_NormalArray[nnormal][count++] = v1;
          m_NormalArray[nnormal][count++] = v2;
        }
      }

      ////// Tangent ////////
      NumTangentAttributes = md->TangentSetArray.size();
      m_TangentArrayXSI = new nux::Vec3<float>* [NumTangentAttributes];

      for (int ntangent = 0; ntangent < NumTangentAttributes; ntangent++)
      {
        m_TangentArrayXSI[ntangent] = new nux::Vec3<float> [nbTriangle*3];

        int count = 0;
        nux::Vec3<float> v0, v1, v2;

        for (long j = 0; j < nbTriangle; j++)
        {
          md->TangentSetIndexArray[ntangent]->IndexPool[j].i;
          v0 = md->TangentSetArray[ntangent]->Pool[md->TangentSetIndexArray[ntangent]->IndexPool[j].i];
          v1 = md->TangentSetArray[ntangent]->Pool[md->TangentSetIndexArray[ntangent]->IndexPool[j].j];
          v2 = md->TangentSetArray[ntangent]->Pool[md->TangentSetIndexArray[ntangent]->IndexPool[j].k];

          m_TangentArrayXSI[ntangent][count++] = v0;
          m_TangentArrayXSI[ntangent][count++] = v1;
          m_TangentArrayXSI[ntangent][count++] = v2;
        }
      }

      ////// Binormal ////////
      NumBinormalAttributes = md->BinormalSetArray.size();
      m_BinormalArrayXSI = new nux::Vec3<float>* [NumBinormalAttributes];

      for (int nbinormal = 0; nbinormal < NumBinormalAttributes; nbinormal++)
      {
        m_BinormalArrayXSI[nbinormal] = new nux::Vec3<float> [nbTriangle*3];

        int count = 0;
        nux::Vec3<float> v0, v1, v2;

        for (long j = 0; j < nbTriangle; j++)
        {
          md->BinormalSetIndexArray[nbinormal]->IndexPool[j].i;
          v0 = md->BinormalSetArray[nbinormal]->Pool[md->BinormalSetIndexArray[nbinormal]->IndexPool[j].i];
          v1 = md->BinormalSetArray[nbinormal]->Pool[md->BinormalSetIndexArray[nbinormal]->IndexPool[j].j];
          v2 = md->BinormalSetArray[nbinormal]->Pool[md->BinormalSetIndexArray[nbinormal]->IndexPool[j].k];

          m_BinormalArrayXSI[nbinormal][count++] = v0;
          m_BinormalArrayXSI[nbinormal][count++] = v1;
          m_BinormalArrayXSI[nbinormal][count++] = v2;
        }
      }

      ////// Texture ////////
      NumTextureAttributes = md->TextureSetArray.size();
      m_TextureArray = new nux::Vec2<float>* [NumTextureAttributes];

      for (int ntex = 0; ntex < NumTextureAttributes; ntex++)
      {
        m_TextureArray[ntex] = new nux::Vec2<float>[nbTriangle*3];

        int count = 0;
        nux::Vec2<float> v0, v1, v2;

        for (long j = 0; j < nbTriangle; j++)
        {
          v0 = md->TextureSetArray[ntex]->Pool[md->TextureSetIndexArray[ntex]->IndexPool[j].i];
          v1 = md->TextureSetArray[ntex]->Pool[md->TextureSetIndexArray[ntex]->IndexPool[j].j];
          v2 = md->TextureSetArray[ntex]->Pool[md->TextureSetIndexArray[ntex]->IndexPool[j].k];

          m_TextureArray[ntex][count++] = v0;
          m_TextureArray[ntex][count++] = v1;
          m_TextureArray[ntex][count++] = v2;
        }
      }

      if (HasNormal() && HasTexCoord() )
      {
        m_TangentArrayCustom = new nux::Vec3<float>[nbTriangle*3];
        m_BinormalArrayCustom = new nux::Vec3<float>[nbTriangle*3];

        CalculateTangentArray (m_VertexArray,
                               m_NormalArray[0],
                               m_TextureArray[0],
                               nbTriangle,
                               &m_IndexArray[0],
                               m_TangentArrayCustom,
                               m_BinormalArrayCustom);
        m_hasTangentSpace = true;
      }
    }
  }

  void NMeshObject::CalculateTangentArray (const nux::Vec4<float> *vertex,
      const nux::Vec3<float> *normal,
      const nux::Vec2<float> *texcoord,
      long triangleCount,
      const unsigned int *triangleIndex,
      nux::Vec3<float> *tangent,
      nux::Vec3<float> *binormal)
  {
    // Compute Tangent and Binormal from just the normal and the texture coordinates.
    // See http://www.terathon.com/code/tangent.html

    int vertexCount = triangleCount * 3;
    nux::Vec3<float> *tan1 = new nux::Vec3<float>[vertexCount * 2];
    nux::Vec3<float> *tan2 = tan1 + vertexCount;
    memset (tan1, 0, vertexCount * sizeof (nux::Vec3<float>) * 2);

    for (long a = 0; a < triangleCount; a++)
    {
      long i1 = triangleIndex[a*3];
      long i2 = triangleIndex[a*3+1];
      long i3 = triangleIndex[a*3+2];

      const nux::Vec4<float>& v1 = vertex[i1];
      const nux::Vec4<float>& v2 = vertex[i2];
      const nux::Vec4<float>& v3 = vertex[i3];

      const nux::Vec2<float>& w1 = texcoord[i1];
      const nux::Vec2<float>& w2 = texcoord[i2];
      const nux::Vec2<float>& w3 = texcoord[i3];

      float x1 = v2.x - v1.x;
      float x2 = v3.x - v1.x;
      float y1 = v2.y - v1.y;
      float y2 = v3.y - v1.y;
      float z1 = v2.z - v1.z;
      float z2 = v3.z - v1.z;

      float s1 = w2.x - w1.x;
      float s2 = w3.x - w1.x;
      float t1 = w2.y - w1.y;
      float t2 = w3.y - w1.y;

      float r = 1.0F / (s1 * t2 - s2 * t1);
      nux::Vec3<float> sdir ( (t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
      nux::Vec3<float> tdir ( (s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

      tan1[i1] += sdir;
      tan1[i2] += sdir;
      tan1[i3] += sdir;

      tan2[i1] += tdir;
      tan2[i2] += tdir;
      tan2[i3] += tdir;
    }

    long count = vertexCount;

    for (long a = 0; a < count; a++)
    {
      const nux::Vec3<float>& n = normal[a];
      const nux::Vec3<float>& t1 = tan1[a];
      const nux::Vec3<float>& t2 = tan2[a];

      nux::Vector3 N (n.x, n.y, n.z);
      nux::Vector3 T1 (t1.x, t1.y, t1.z);
      nux::Vector3 T2 (t2.x, t2.y, t2.z);

      // Gram-Schmidt orthogonalize
      nux::Vector3 Tangent = T1 - (N.DotProduct (T1) ) * N;
      Tangent.Normalize();
      tangent[a] = nux::Vec3<float> (Tangent.x, Tangent.y, Tangent.z);

      // Calculate handedness
      float handness = ( ( (N.CrossProduct (T1) ).DotProduct (T2) ) < 0.0F) ? -1.0F : 1.0F;

      nux::Vector3 Binormal = handness * N.CrossProduct (Tangent);
      binormal[a] = nux::Vec3<float> (Binormal.x, Binormal.y, Binormal.z);

      binormal[a].Normalize();
    }

    delete[] tan1;
  }

  unsigned int NMeshObject::GetVertexBufferSize()
  {
    return m_NumPolygon * 3 * sizeof (float) * 4; // each vertex is 4 floats (X, Y, Z, W)
  }

  unsigned int NMeshObject::GetNormalBufferSize()
  {
    return m_NumPolygon * 3 * sizeof (float) * 3; // each vertex is 3 floats (X, Y, Z)
  }

  unsigned int NMeshObject::GetColorBufferSize()
  {
    return m_NumPolygon * 3 * sizeof (float) * 4; // RGBA!
  }

  unsigned int NMeshObject::GetTextureBufferSize()
  {
    return m_NumPolygon * 3 * sizeof (float) * 2; // each UV is 2 floats
  }

  unsigned int NMeshObject::GetIndexBufferSize()
  {
    return m_NumPolygon * 3 * sizeof (int);
  }

  unsigned int NMeshObject::GetNumVertex()
  {
    return m_NumPolygon * 3;
  }

  unsigned int NMeshObject::GetNumIndex()
  {
    return m_NumPolygon * 3;
  }


  void NMeshObject::CreateGLObjects()
  {
    int size;

    if (m_VertexArray == 0)
      return;

    int Index = 1; // 0 reserved for vertex position
    // vertex data
    size = GetVertexBufferSize();

    CHECKGLX ( glGenBuffersARB (1, &m_OpenGLID[0]) );
    CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, m_OpenGLID[0]) );
    CHECKGLX ( glBufferDataARB (GL_ARRAY_BUFFER_ARB, size, & (m_VertexArray[0]), GL_STATIC_DRAW_ARB) );

    // texture data
    if (HasTexCoord() )
    {
      m_TexCoordIndex = Index++;
      size = GetTextureBufferSize();
      // Generate And Bind The Texture Coordinate Buffer
      CHECKGLX ( glGenBuffersARB ( 1, &m_OpenGLID[m_TexCoordIndex]) );
      CHECKGLX ( glBindBufferARB ( GL_ARRAY_BUFFER_ARB, m_OpenGLID[m_TexCoordIndex]) );
      CHECKGLX ( glBufferDataARB ( GL_ARRAY_BUFFER_ARB, size, m_TextureArray[0], GL_STATIC_DRAW_ARB) );
    }

    // Normal data
    if (HasNormal() )
    {
      m_NormalIndex = Index++;
      size = GetNormalBufferSize();
      CHECKGLX ( glGenBuffersARB (1, &m_OpenGLID[m_NormalIndex]) );
      CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, m_OpenGLID[m_NormalIndex]) );
      CHECKGLX ( glBufferDataARB (GL_ARRAY_BUFFER_ARB, size, m_NormalArray[0], GL_STATIC_DRAW_ARB) );
    }

    if (HasTangentSpace() )
    {
      size = GetNormalBufferSize();
      CHECKGLX ( glGenBuffersARB (1, &m_GLVBOTangentCustom) );
      CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, m_GLVBOTangentCustom) );
      CHECKGLX ( glBufferDataARB (GL_ARRAY_BUFFER_ARB, size, m_TangentArrayCustom, GL_STATIC_DRAW_ARB) );


      size = GetNormalBufferSize();
      CHECKGLX ( glGenBuffersARB (1, &m_GLVBOBiNormalCustom) );
      CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, m_GLVBOBiNormalCustom) );
      CHECKGLX ( glBufferDataARB (GL_ARRAY_BUFFER_ARB, size, m_BinormalArrayCustom, GL_STATIC_DRAW_ARB) );
    }

    if (HasTangent() )
    {
      m_TangentIndex = Index++;

      size = GetNormalBufferSize();
      CHECKGLX ( glGenBuffersARB (1, &m_OpenGLID[m_TangentIndex]) );
      CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, m_OpenGLID[m_TangentIndex]) );
      CHECKGLX ( glBufferDataARB (GL_ARRAY_BUFFER_ARB, size, m_TangentArrayXSI[0], GL_STATIC_DRAW_ARB) );
    }

    if (HasBinormal() )
    {
      m_BinormalIndex = Index++;
      size = GetNormalBufferSize();
      CHECKGLX ( glGenBuffersARB (1, &m_OpenGLID[m_BinormalIndex]) );
      CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, m_OpenGLID[m_BinormalIndex]) );
      CHECKGLX ( glBufferDataARB (GL_ARRAY_BUFFER_ARB, size, m_BinormalArrayXSI[0], GL_STATIC_DRAW_ARB) );
    }

    if (HasColor() )
    {
      m_Color0Index = Index++;
      size = GetColorBufferSize();
      CHECKGLX ( glGenBuffersARB (1, &m_OpenGLID[m_Color0Index]) );
      CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, m_OpenGLID[m_Color0Index]) );
      CHECKGLX ( glBufferDataARB (GL_ARRAY_BUFFER_ARB, size, m_ColorArray[0], GL_STATIC_DRAW_ARB) );
    }

    // index data
    size = GetIndexBufferSize();
    CHECKGLX ( glGenBuffersARB ( 1, &m_GLVBOIndexVertex) );
    CHECKGLX ( glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, m_GLVBOIndexVertex) );
    CHECKGLX ( glBufferDataARB (GL_ELEMENT_ARRAY_BUFFER_ARB, size, &m_IndexArray[0], GL_STATIC_DRAW_ARB) );
  }

  void NMeshObject::DrawGLObjects()
  {
    if (m_VertexArray == 0)
      return;

    CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, GetOpenGLId (0) ) );
    CHECKGLX ( glEnableClientState (GL_VERTEX_ARRAY) );
    CHECKGLX ( glVertexPointer (4, GL_FLOAT, 0, NULL) );

    if (HasTexCoord() )
    {
      CHECKGLX ( glClientActiveTextureARB (GL_TEXTURE0) );
      CHECKGLX ( glBindBufferARB ( GL_ARRAY_BUFFER_ARB, GetOpenGLId (m_TexCoordIndex) ) );
      CHECKGLX ( glEnableClientState (GL_TEXTURE_COORD_ARRAY) );
      CHECKGLX ( glTexCoordPointer (2, GL_FLOAT, 0, NULL) );
    }

    if (HasNormal() )
    {
      CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, GetOpenGLId (m_NormalIndex) ) );
      CHECKGLX ( glEnableClientState (GL_NORMAL_ARRAY) );
      CHECKGLX ( glNormalPointer (GL_FLOAT, 0, NULL) );
    }

    if (HasColor() )
    {
      CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, GetOpenGLId (m_Color0Index) ) );
      CHECKGLX ( glEnableClientState (GL_COLOR_ARRAY) );
      CHECKGLX ( glColorPointer (4, GL_FLOAT, 0, NULL) );
    }

    if (HasTangentSpace() )
    {
      CHECKGLX ( glClientActiveTextureARB (GL_TEXTURE1_ARB) );
      CHECKGLX ( glEnableClientState (GL_TEXTURE_COORD_ARRAY) );
      CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, m_GLVBOTangentCustom) );
      CHECKGLX ( glTexCoordPointer (3, GL_FLOAT, 0, NULL) );

      CHECKGLX ( glClientActiveTextureARB (GL_TEXTURE2_ARB) );
      CHECKGLX ( glEnableClientState (GL_TEXTURE_COORD_ARRAY) );
      CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, m_GLVBOBiNormalCustom) );
      CHECKGLX ( glTexCoordPointer (3, GL_FLOAT, 0, NULL) );
    }

    if (HasTangent() )
    {
      CHECKGLX ( glClientActiveTextureARB (GL_TEXTURE3_ARB) );
      CHECKGLX ( glEnableClientState (GL_TEXTURE_COORD_ARRAY) );
      CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, GetOpenGLId (m_TangentIndex) ) );
      CHECKGLX ( glTexCoordPointer (3, GL_FLOAT, 0, NULL) );
    }

    if (HasBinormal() )
    {
      CHECKGLX ( glClientActiveTextureARB (GL_TEXTURE4_ARB) );
      CHECKGLX ( glEnableClientState (GL_TEXTURE_COORD_ARRAY) );
      CHECKGLX ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, GetOpenGLId (m_BinormalIndex) ) );
      CHECKGLX ( glTexCoordPointer (3, GL_FLOAT, 0, NULL) );
    }

    CHECKGLX ( glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, m_GLVBOIndexVertex) );

    CHECKGLX ( glDrawElements ( GL_TRIANGLES, GetNumVertex(),
                                GL_UNSIGNED_INT, NULL) );		// Draw All Of The Triangles At Once

    //    GLint aaa;
    //    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &aaa);
    //
    //    glDrawRangeElements(GL_TRIANGLES,
    //        m_ArrayStats[0].MinIndex,
    //        m_ArrayStats[0].MaxIndex,
    //        m_ArrayStats[0].nbTriangles * 3, // if rendering GL_TRIANGLES, then the number of index is nbTriangles * 3
    //        GL_UNSIGNED_INT, 0);

    CHECKGLX ( glDisableClientState (GL_VERTEX_ARRAY) );
    CHECKGLX ( glDisableClientState (GL_INDEX_ARRAY) );

    if (HasTexCoord() )
    {
      CHECKGLX ( glDisableClientState (GL_TEXTURE_COORD_ARRAY) );
    }

    if (HasNormal() )
    {
      CHECKGLX ( glDisableClientState (GL_NORMAL_ARRAY) );
    }

    if (HasColor() )
    {
      CHECKGLX ( glDisableClientState (GL_COLOR_ARRAY) );
    }

    CHECKGLX ( glActiveTextureARB (GL_TEXTURE0) );
    CHECKGLX ( glDisableClientState (GL_TEXTURE_COORD_ARRAY) );
    CHECKGLX ( glActiveTextureARB (GL_TEXTURE1_ARB) );
    CHECKGLX ( glDisableClientState (GL_TEXTURE_COORD_ARRAY) );
    CHECKGLX ( glActiveTextureARB (GL_TEXTURE2_ARB) );
    CHECKGLX ( glDisableClientState (GL_TEXTURE_COORD_ARRAY) );
    CHECKGLX ( glActiveTextureARB (GL_TEXTURE3_ARB) );
    CHECKGLX ( glDisableClientState (GL_TEXTURE_COORD_ARRAY) );
    CHECKGLX ( glActiveTextureARB (GL_TEXTURE4_ARB) );
    CHECKGLX ( glDisableClientState (GL_TEXTURE_COORD_ARRAY) );
  }

  void NMeshObject::RenderNormal()
  {
    // Push the mesh local to world matrix.
    glPushMatrix();

    {
      int nbTriangle = m_NumPolygon;

      for (long j = 0; j < nbTriangle; j++)
      {
        glColor4f (0.0f, 0.0f, 1.0f, 1.0f);
        glBegin (GL_LINES);
        glVertex3f (m_VertexArray[3*j].x, m_VertexArray[3*j].y, m_VertexArray[3*j].z);
        glVertex3f (m_VertexArray[3*j].x + m_NormalArray[0][3*j].x,
                    m_VertexArray[3*j].y + m_NormalArray[0][3*j].y,
                    m_VertexArray[3*j].z + m_NormalArray[0][3*j].z);

        glVertex3f (m_VertexArray[3*j+1].x, m_VertexArray[3*j+1].y, m_VertexArray[3*j+1].z);
        glVertex3f (m_VertexArray[3*j+1].x + m_NormalArray[0][3*j+1].x,
                    m_VertexArray[3*j+1].y + m_NormalArray[0][3*j+1].y,
                    m_VertexArray[3*j+1].z + m_NormalArray[0][3*j+1].z);

        glVertex3f (m_VertexArray[3*j+2].x, m_VertexArray[3*j+2].y, m_VertexArray[3*j+2].z);
        glVertex3f (m_VertexArray[3*j+2].x + m_NormalArray[0][3*j+2].x,
                    m_VertexArray[3*j+2].y + m_NormalArray[0][3*j+2].y,
                    m_VertexArray[3*j+2].z + m_NormalArray[0][3*j+2].z);

        glEnd();

        if (HasBinormal() )
        {
          glColor4f (0.0f, 1.0f, 0.0f, 1.0f);
          glBegin (GL_LINES);
          glVertex3f (m_VertexArray[3*j].x, m_VertexArray[3*j].y, m_VertexArray[3*j].z);
          glVertex3f (m_VertexArray[3*j].x + m_BinormalArrayXSI[0][3*j].x,
                      m_VertexArray[3*j].y + m_BinormalArrayXSI[0][3*j].y,
                      m_VertexArray[3*j].z + m_BinormalArrayXSI[0][3*j].z);

          glVertex3f (m_VertexArray[3*j+1].x, m_VertexArray[3*j+1].y, m_VertexArray[3*j+1].z);
          glVertex3f (m_VertexArray[3*j+1].x + m_BinormalArrayXSI[0][3*j+1].x,
                      m_VertexArray[3*j+1].y + m_BinormalArrayXSI[0][3*j+1].y,
                      m_VertexArray[3*j+1].z + m_BinormalArrayXSI[0][3*j+1].z);

          glVertex3f (m_VertexArray[3*j+2].x, m_VertexArray[3*j+2].y, m_VertexArray[3*j+2].z);
          glVertex3f (m_VertexArray[3*j+2].x + m_BinormalArrayXSI[0][3*j+2].x,
                      m_VertexArray[3*j+2].y + m_BinormalArrayXSI[0][3*j+2].y,
                      m_VertexArray[3*j+2].z + m_BinormalArrayXSI[0][3*j+2].z);

          glEnd();
        }

        if (HasTangent() )
        {
          glColor4f (1.0f, 0.0f, 0.0f, 1.0f);
          glBegin (GL_LINES);
          glVertex3f (m_VertexArray[3*j].x, m_VertexArray[3*j].y, m_VertexArray[3*j].z);
          glVertex3f (m_VertexArray[3*j].x + m_TangentArrayXSI[0][3*j].x,
                      m_VertexArray[3*j].y + m_TangentArrayXSI[0][3*j].y,
                      m_VertexArray[3*j].z + m_TangentArrayXSI[0][3*j].z);

          glVertex3f (m_VertexArray[3*j+1].x, m_VertexArray[3*j+1].y, m_VertexArray[3*j+1].z);
          glVertex3f (m_VertexArray[3*j+1].x + m_TangentArrayXSI[0][3*j+1].x,
                      m_VertexArray[3*j+1].y + m_TangentArrayXSI[0][3*j+1].y,
                      m_VertexArray[3*j+1].z + m_TangentArrayXSI[0][3*j+1].z);

          glVertex3f (m_VertexArray[3*j+2].x, m_VertexArray[3*j+2].y, m_VertexArray[3*j+2].z);
          glVertex3f (m_VertexArray[3*j+2].x + m_TangentArrayXSI[0][3*j+2].x,
                      m_VertexArray[3*j+2].y + m_TangentArrayXSI[0][3*j+2].y,
                      m_VertexArray[3*j+2].z + m_TangentArrayXSI[0][3*j+2].z);

          glEnd();
        }
      }
    }

    // Pop the mesh local to world matrix.
    glPopMatrix();
  }

  nux::Matrix4 &NMeshObject::GetMatrix()
  {
    return m_WorldMatrix;
  }

  const nux::Matrix4 &NMeshObject::GetMatrix() const
  {
    return m_WorldMatrix;
  }

}
