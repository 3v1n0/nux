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


#include "NuxCore/NuxCore.h"
#include "N3DWorld.h"
#include "NMeshObject.h"

namespace nux
{

  N3DWorld::N3DWorld()
  {

  }

  N3DWorld::~N3DWorld()
  {

  }

  void N3DWorld::AddMesh (NMeshData *mesh)
  {
    if (mesh == 0)
      return;

    NMeshObject *meshobject = new NMeshObject (mesh);
    m_MeshObjectArray.push_back (meshobject);
  }

  void N3DWorld::AddMeshObject (NMeshObject *mesh)
  {
    if (mesh == 0)
      return;

    if (std::find (m_MeshObjectArray.begin(), m_MeshObjectArray.end(), mesh) != m_MeshObjectArray.end() )
      return;

    m_MeshObjectArray.push_back (mesh);
  }

  int N3DWorld::NumMesh() const
  {
    return m_MeshObjectArray.size();
  }

  void N3DWorld::RenderWorld()
  {
    int NumMesh = m_MeshObjectArray.size();

    for (int i = 0; i < NumMesh; i++)
    {
      m_MeshObjectArray[i]->DrawGLObjects();
    }
  }

  void N3DWorld::Flush()
  {
    int NumMesh = m_MeshObjectArray.size();

    for (int i = 0; i < NumMesh; i++)
    {
      delete m_MeshObjectArray[i];
    }

    m_MeshObjectArray.clear();
  }

}


