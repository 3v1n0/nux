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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef GLVERTEXRESOURCEMANAGER_H
#define GLVERTEXRESOURCEMANAGER_H

#include "GLResourceManager.h"
#include "IOpenGLVertexBuffer.h"
#include "IOpenGLIndexBuffer.h"
#include "IOpenGLVertexDeclaration.h"
#include "NuxMesh/NMeshObject.h"

namespace nux
{

  class NMeshObject;

  class NVertexBuffer: public ResourceData
  {
    NUX_DECLARE_OBJECT_TYPE (NVertexBuffer, ResourceData);

  public:
    NVertexBuffer();
    virtual ~NVertexBuffer();
    NVertexBuffer (int Size, int Stride);
    //! Copy constructor
    NVertexBuffer (const NVertexBuffer &);
    //! Assignment constructor
    NVertexBuffer &operator = (const NVertexBuffer &);

    void Allocate (int Size, int Stride);
    int GetSize() const;
    int GetStride() const;
    int GetNumElement() const;
    const t_u8 *GetPtrRawData() const;
    t_u8 *GetPtrRawData();

  public:
    std::vector<t_u8> _Data;
    int _Stride;
  };

  class NIndexBuffer: public ResourceData
  {
    NUX_DECLARE_OBJECT_TYPE (NIndexBuffer, ResourceData);

    // NIndexBuffer is very similar to NVertexBuffer except that vertex indices
    // are not inter-mixed with other vertex data. So the Stride here should be 2 bytes or 4 bytes.
  public:
    NIndexBuffer();
    virtual ~NIndexBuffer();
    NIndexBuffer (int Size, int Stride);
    //! Copy constructor
    NIndexBuffer (const NIndexBuffer &);
    //! Assignment constructor
    NIndexBuffer &operator = (const NIndexBuffer &);

    void Allocate (int Size, int Stride);
    int GetSize() const;
    int GetStride() const;
    int GetNumElement() const;
    const t_u8 *GetPtrRawData() const;
    t_u8 *GetPtrRawData();

  public:
    std::vector<t_u8> _Data;
    int _Stride;
  };

  class NVertexDeclaration: public ResourceData
  {
    NUX_DECLARE_OBJECT_TYPE (NVertexDeclaration, ResourceData);

  public:
    NVertexDeclaration();
    ~NVertexDeclaration();
    void AddVertexComponent (VERTEXELEMENT);
    std::vector<VERTEXELEMENT> m_Declaration;
  };

  class NGLVertexBuffer: public CachedResourceData
  {
    NUX_DECLARE_OBJECT_TYPE (NGLVertexBuffer, CachedResourceData);

  public:
    NGLVertexBuffer (NResourceSet *ResourceManager, NVertexBuffer *VertexData);
    ~NGLVertexBuffer ();

    ObjectPtr<IOpenGLVertexBuffer>	m_VtxBuffer;

    bool UpdateResource (ResourceData *Resource);
  private:
    void LoadVertexData (NVertexBuffer *VertexData);
    int _Size;
    int _Stride;
  };

  class NGLIndexBuffer: public CachedResourceData
  {
    NUX_DECLARE_OBJECT_TYPE (NGLIndexBuffer, CachedResourceData);
  public:
    NGLIndexBuffer (NResourceSet *ResourceManager, NIndexBuffer *Resource);
    ~NGLIndexBuffer();

    ObjectPtr<IOpenGLIndexBuffer>	m_IdxBuffer;

    bool UpdateResource (ResourceData *Resource);
  private:
    void LoadIndexData (NIndexBuffer *VertexData);
    int _Size;
    int _Stride;
  };

  class NGLVertexDeclaration: public CachedResourceData
  {
    NUX_DECLARE_OBJECT_TYPE (NGLVertexDeclaration, CachedResourceData);
  public:
    NGLVertexDeclaration (NResourceSet *ResourceManager, NVertexDeclaration *Resource);
    ~NGLVertexDeclaration();

    bool UpdateResource (ResourceData *Resource);
    ObjectPtr< IOpenGLVertexDeclaration > m_VtxDeclaration;
  };

  class NMeshComponent
  {
  public:
    NMeshComponent();
    NMeshComponent (const NMeshComponent &);
    NMeshComponent (int StreamIndex/*ObjectPtr<NVertexBuffer> VtxBuffer*/, int Offset, ATTRIB_DECL_TYPE Type);

    NMeshComponent &operator = (const NMeshComponent &);

    virtual ~NMeshComponent();

    int GetStreamIndex() const;
//     int GetStride();
    int GetOffset();
  private:
    ATTRIB_DECL_TYPE _Type;
    int _StreamIndex;
    int _Offset;
  };

  class NMesh: public ResourceData
  {
    NUX_DECLARE_OBJECT_TYPE (NStaticMesh, ResourceData);

    NMesh();
    virtual ~NMesh();
  };

  class NStaticMesh: public NMesh
  {
    NUX_DECLARE_OBJECT_TYPE (NStaticMesh, NMesh);
  public:
    NStaticMesh (NMeshObject *Object);
    ~NStaticMesh();

    int GetNumStreams() const;

    std::vector< NVertexBuffer * > m_pVertexStreamArray; // up to 8 stream of buffers on most GPU
    NIndexBuffer *m_pIndex;
    NVertexDeclaration *m_pVertexDeclaration;
  };

  class NGLStaticMesh: public CachedResourceData
  {
    NUX_DECLARE_OBJECT_TYPE (NGLStaticMesh, CachedResourceData);
  public:
    NGLStaticMesh (NResourceSet *ResourceManager, NStaticMesh *);
    ~NGLStaticMesh ();
    bool UpdateResource (ResourceData *Resource);

    std::vector< ObjectPtr<NGLVertexBuffer> > m_VertexBufferArray;
    ObjectPtr<NGLIndexBuffer> m_Index;
    ObjectPtr<NGLVertexDeclaration> m_VertexDeclaration;
  };

}

#endif // GLVERTEXRESOURCEMANAGER_H
