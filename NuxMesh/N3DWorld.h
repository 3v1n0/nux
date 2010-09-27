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


#ifndef N3DWORLD_H
#define N3DWORLD_H

#include "NuxCore/NKernel.h"
#include "NuxCore/NNamespace.h"
#include "NuxCore/Math/Matrix4.h"

namespace nux { //NUX_NAMESPACE_BEGIN
class NMeshData;
class NMeshObject;

class N3DWorld
{
public:
    N3DWorld();
    ~N3DWorld();

    int NumMesh() const;
    void AddMesh(NMeshData* mesh);
    void AddMeshObject(NMeshObject* mesh);
    void Flush();

    void RenderWorld();
    NMeshObject* GetMesh(int index) {return m_MeshObjectArray[index];}

    void SetTransformMatrix(nux::Matrix4 matrix) {m_CurrentTransformMatrix = matrix;}
    nux::Matrix4& GetTransformMatrix() { return m_CurrentTransformMatrix;}
    const nux::Matrix4& GetTransformMatrix() const { return m_CurrentTransformMatrix;}

private:
    nux::Matrix4 m_CurrentTransformMatrix;
    std::vector<NMeshObject*> m_MeshObjectArray;
};

} //NUX_NAMESPACE_END

#endif // N3DWORLD_H

