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



#include "SceneData.h"



MeshData2::MeshData2()
{
  bInitialized            = false;
  szModelName[0]          = 0;
  vTranslation            = nux::Vector3 (0.0f, 0.0f, 0.0f);
  vRotation               = nux::Vector3 (0.0f, 0.0f, 0.0f);
  vScaling                = nux::Vector3 (1.0f, 1.0f, 1.0f);
  matWorld                = nux::Matrix4();
  VertexPool              = NULL;
  NormalPool              = NULL;
  ColorPool               = NULL;
  UVPool                  = NULL;
  nbUVSets                = 0;        // nb texture
  nbUVInSet               = NULL;     // nb UV in Texture // this is an array of nbUVSets elements
  nbVertex                = 0;
  nbColor                 = 0;
  nbNormal                = 0;

  m_SubMeshCount                  = 0;
  m_SubMeshTriangleCount          = NULL;
  VertexIndexArray                = NULL;
  ColorIndexArray                 = NULL;
  NormalIndexArray                = NULL;
  UVIndexArray                    = NULL;

  nbIndexInVertexArray            = NULL; ;
  nbIndexInColorlArray            = NULL; ;
  nbIndexInNormalArray            = NULL; ;
  nbIndexInUVArray                = NULL; ;
}

MeshData2::~MeshData2()
{
  std::vector<MeshData2 *>::iterator it;

  for (it = m_ChildArray.begin(); it != m_ChildArray.end(); it++)
  {
    delete (*it);
  }
}

MeshData3::MeshData3()
{
  bInitialized            = false;
  szModelName[0]          = 0;
  vTranslation            = nux::Vector3 (0.0f, 0.0f, 0.0f);
  vRotation               = nux::Vector3 (0.0f, 0.0f, 0.0f);
  vScaling                = nux::Vector3 (1.0f, 1.0f, 1.0f);
  matWorld                = nux::Matrix4();
  VertexPool              = NULL;
  NormalPool              = NULL;
  ColorPool               = NULL;
  UVPool                  = NULL;
  nbUVSets                = 0;        // nb texture
  nbUVInSet               = NULL;     // nb UV in Texture // this is an array of nbUVSets elements
  nbVertex                = 0;
  nbColor                 = 0;
  nbNormal                = 0;

  m_SubMeshCount                  = 0;
  m_SubMeshTriangleCount          = NULL;
  VertexIndexArray                = NULL;
//    ColorIndexArray                 = NULL;
//    NormalIndexArray                = NULL;
//    UVIndexArray                    = NULL;

  nbIndexInVertexArray            = NULL; ;
//    nbIndexInColorlArray            = NULL; ;
//    nbIndexInNormalArray            = NULL; ;
//    nbIndexInUVArray                = NULL; ;
}

MeshData3::~MeshData3()
{
  std::vector<MeshData2 *>::iterator it;

  for (it = m_ChildArray.begin(); it != m_ChildArray.end(); it++)
  {
    delete (*it);
  }
}

