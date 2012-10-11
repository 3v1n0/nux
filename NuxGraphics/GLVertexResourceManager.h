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


#ifndef GLVERTEXRESOURCEMANAGER_H
#define GLVERTEXRESOURCEMANAGER_H

#include "GLResourceManager.h"
#include "IOpenGLVertexBuffer.h"
#include "IOpenGLIndexBuffer.h"
#include "IOpenGLVertexDeclaration.h"
#include "MeshData.h"

namespace nux
{
  class VertexBuffer: public ResourceData
  {
    NUX_DECLARE_OBJECT_TYPE(VertexBuffer, ResourceData);

  public:
    VertexBuffer();
    virtual ~VertexBuffer();
    VertexBuffer(int Size, int Stride);

    //! Allocate space for a "num_element", each of size "size".
    /*!
        @param num_element Number of elements.
        @param size Number of byte for every element.
    */
    void Allocate(int num_index, int size);
    int GetSize() const;
    int GetStride() const;
    int GetNumElement() const;
    const void* GetPtrRawData() const;
    void* GetPtrRawData();

  public:
    std::vector<unsigned char> _Data;
    int _Stride;
  };

  class IndexBuffer: public ResourceData
  {
    NUX_DECLARE_OBJECT_TYPE(IndexBuffer, ResourceData);

    // IndexBuffer is very similar to MeshBuffer except that vertex indices
    // are not inter-mixed with other vertex data. So the Stride here should be 2 bytes or 4 bytes.
  public:
    IndexBuffer();
    virtual ~IndexBuffer();
    IndexBuffer(int Size, int Stride);

    void Allocate(int Size, int Stride);
    int GetSize() const;
    int GetStride() const;
    int GetNumIndex() const;
    const void* GetPtrRawData() const;
    void* GetPtrRawData();

  public:
    std::vector<int> _Data;
    int _Stride;
    int _num_index;
  };

  class VertexDeclaration: public ResourceData
  {
    NUX_DECLARE_OBJECT_TYPE(NVertexDeclaration, ResourceData);

  public:
    VertexDeclaration();
    ~VertexDeclaration();
    void AddVertexComponent(VERTEXELEMENT);
    std::vector<VERTEXELEMENT> _declaration;
  };

  class CachedVertexBuffer: public CachedResourceData
  {
    NUX_DECLARE_OBJECT_TYPE(CachedVertexBuffer, CachedResourceData);
  public:
    CachedVertexBuffer(NResourceSet* ResourceManager, VertexBuffer* resource);
    ~CachedVertexBuffer();

    ObjectPtr<IOpenGLVertexBuffer>	_vertex_buffer;

    bool UpdateResource(ResourceData* Resource);
    int GetElementSize() const;
    int GetBufferStride() const;

  private:
    void LoadVertexData(VertexBuffer* vertex_buffer);
    int _Size;
    int _Stride;
  };

  class CachedIndexBuffer: public CachedResourceData
  {
    NUX_DECLARE_OBJECT_TYPE(CachedIndexBuffer, CachedResourceData);
  public:
    CachedIndexBuffer(NResourceSet* ResourceManager, IndexBuffer* resource);
    ~CachedIndexBuffer();

    ObjectPtr<IOpenGLIndexBuffer>	_index_buffer;

    bool UpdateResource(ResourceData* Resource);
    int GetElementSize() const;
    int GetBufferStride() const;
    int GetNumIndex() const;

  private:
    void LoadIndexData(IndexBuffer* index_buffer);
    int _Size;
    int _Stride;
    int _num_index;
  };

  class CachedVertexDeclaration: public CachedResourceData
  {
    NUX_DECLARE_OBJECT_TYPE(CachedVertexDeclaration, CachedResourceData);
  public:
    CachedVertexDeclaration(NResourceSet* ResourceManager, VertexDeclaration* Resource);
    ~CachedVertexDeclaration();

    bool UpdateResource(ResourceData* Resource);
    ObjectPtr<IOpenGLVertexDeclaration> _declaration;
  };

  class CachedMeshBuffer;

  class MeshBuffer: public ResourceData
  {
    NUX_DECLARE_OBJECT_TYPE(MeshBuffer, ResourceData);
  public:
    MeshBuffer(NUX_FILE_LINE_PROTO);
    virtual ~MeshBuffer();

    bool Update(const MeshData* mesh_data);

    ObjectPtr<CachedMeshBuffer> GetCachedMeshBuffer();

  public:
    VertexBuffer*       _vertex_buffer;
    IndexBuffer*        _index_buffer;
    VertexDeclaration*  _vertex_declaration;
    NuxMeshPrimitiveType _mesh_primitive_type;
  };

  class CachedMeshBuffer: public CachedResourceData
  {
    NUX_DECLARE_OBJECT_TYPE(CachedMeshBuffer, CachedResourceData);
  public:
    ObjectPtr<CachedVertexBuffer> _cached_vertex_buffer;
    ObjectPtr<CachedVertexDeclaration> _cached_vertex_declaration;
    ObjectPtr<CachedIndexBuffer> _cached_index_buffer;
    NuxMeshPrimitiveType _mesh_primitive_type;

    CachedMeshBuffer(NResourceSet* ResourceManager, MeshBuffer* resource);
    ~CachedMeshBuffer();

    bool UpdateResource(ResourceData* Resource);
  };
}

#endif // GLVERTEXRESOURCEMANAGER_H
