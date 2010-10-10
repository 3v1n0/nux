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


#ifndef SCENEDATA_H
#define SCENEDATA_H

#include "NuxCore/NuxCore.h"
#include "NuxCore/Math/Matrix4.h"
#include "NuxCore/Math/Vector2.h"
#include "NuxCore/Math/Vector3.h"
#include "NuxCore/Math/Vector4.h"
#include "NuxCore/Math/Constants.h"

#include "NuxCore/Exception.h"

struct Index3
{
  unsigned int i;
  unsigned int j;
  unsigned int k;

  Index3()
  {
  }

  Index3 (unsigned int I, unsigned int J, unsigned int K)
  {
    i = I;
    j = J;
    k = K;
  }
  Index3 &operator = (Index3 &idx)
  {
    i = idx.i;
    j = idx.j;
    k = idx.k;
    return *this;
  }
} ;

class MeshData2
{
public:
  MeshData2();
  ~MeshData2();
  bool bInitialized;			// Is this a valid entry?
  char szModelName[256];
  nux::Vector3 vTranslation;
  nux::Vector3 vRotation;
  nux::Vector3 vScaling;
  nux::Matrix4 matWorld;
  nux::Vec4<float> *VertexPool;
  nux::Vec3<float> *NormalPool;
  nux::Vec4<float> *ColorPool;
  nux::Vec2<float> **UVPool;
  int nbUVSets;  // nb texture
  int *nbUVInSet;  // nb UV in Texture // this is an array of nbUVSets elements
  int nbVertex;
  int nbColor;
  int nbNormal;

  int m_SubMeshCount;
  int *m_SubMeshTriangleCount;
  Index3 **VertexIndexArray;
  Index3 **ColorIndexArray;
  Index3 **NormalIndexArray;
  Index3 *** UVIndexArray;

  int *nbIndexInVertexArray;
  int *nbIndexInColorlArray;
  int *nbIndexInNormalArray;
  int **nbIndexInUVArray;

  /*nux::Vec3<float>** VertexArray;
  nux::Vec3<float>** ColorArray;
  nux::Vec3<float>** NormalArray;
  nux::Vec3<float>** TangentArray;
  nux::Vec3<float>** BinormalArray;
  Float2*** UVArray;
  int** IndexArray; */

  std::vector<MeshData2 *> m_ChildArray;
};

class MeshData3
{
public:
  MeshData3();
  ~MeshData3();
  bool bInitialized;			// Is this a valid entry?
  char szModelName[256];
  nux::Vector3 vTranslation;
  nux::Vector3 vRotation;
  nux::Vector3 vScaling;
  nux::Matrix4 matWorld;
  nux::Vec4<float> *VertexPool;
  nux::Vec3<float> *NormalPool;
  nux::Vec4<float> *ColorPool;
  nux::Vec3<float> **UVPool;
  int nbUVSets;  // nb texture
  int *nbUVInSet;  // nb UV in Texture // this is an array of nbUVSets elements
  int nbVertex;
  int nbColor;
  int nbNormal;

  int m_SubMeshCount;
  int *m_SubMeshTriangleCount;
  Index3 **VertexIndexArray;
//    Index3** ColorIndexArray;
//    Index3** NormalIndexArray;
//    Index3*** UVIndexArray;

  int *nbIndexInVertexArray;
//    int* nbIndexInColorlArray;
//    int* nbIndexInNormalArray;
//    int** nbIndexInUVArray;

  std::vector<MeshData2 *> m_ChildArray;
};

#endif // SCENEDATA_H