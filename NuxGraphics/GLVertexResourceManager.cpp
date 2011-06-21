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


#include "GLResource.h"
#include "GLResourceManager.h"
#include "GLDeviceObjects.h"
#include "GpuDevice.h"
#include "GLVertexResourceManager.h"
#include "GraphicsEngine.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(VertexBuffer);
  NUX_IMPLEMENT_OBJECT_TYPE(IndexBuffer);
  NUX_IMPLEMENT_OBJECT_TYPE(VertexDeclaration);
  NUX_IMPLEMENT_OBJECT_TYPE(MeshBuffer);
  
  NUX_IMPLEMENT_OBJECT_TYPE(CachedVertexBuffer);
  NUX_IMPLEMENT_OBJECT_TYPE(CachedIndexBuffer);
  NUX_IMPLEMENT_OBJECT_TYPE(CachedVertexDeclaration);
  NUX_IMPLEMENT_OBJECT_TYPE(CachedMeshBuffer);

  VertexBuffer::VertexBuffer()
  {
    _Stride = 0;
  }

  VertexBuffer::~VertexBuffer()
  {
    _Data.clear();
  }

  VertexBuffer::VertexBuffer (int num_element, int size)
  {
    Allocate (num_element, size);
  }

  //! Copy constructor
  VertexBuffer::VertexBuffer (const VertexBuffer &Other)
  {
    _Data = Other._Data;
    _Stride = Other._Stride;
  }

  //! Assignment constructor
  VertexBuffer &VertexBuffer::operator = (const VertexBuffer &Other)
  {
    _Data = Other._Data;
    _Stride = Other._Stride;
    return *this;
  }

  void VertexBuffer::Allocate(int num_element, int size)
  {
    nuxAssert(num_element > 0);
    nuxAssert(size > 0);

    if((size <= 0) || (num_element <= 0))
    {
      _Data.clear();
      _Stride = 0;
      return;
    }

    // Make sure Size is a multiple of Stride
    int sz = size * num_element;
    _Stride = size;

    _Data.clear();
    _Data.resize(sz);
  }

  int VertexBuffer::GetSize() const
  {
    return _Data.size();
  }

  int VertexBuffer::GetStride() const
  {
    return _Stride;
  }

  int VertexBuffer::GetNumElement() const
  {
    return _Data.size() / _Stride;
  }

  const void* VertexBuffer::GetPtrRawData() const
  {
    if (_Data.size() == 0)
      return 0;

    return NUX_STATIC_CAST (const void*, &_Data[0]);
  }

  void* VertexBuffer::GetPtrRawData()
  {
    if (_Data.size() == 0)
      return 0;

    return NUX_CONST_CAST(void*, ((NUX_CONST_CAST (const VertexBuffer *, this))->GetPtrRawData()));
  }

  IndexBuffer::IndexBuffer()
  {
    _Stride = 0;
  }

  IndexBuffer::~IndexBuffer()
  {
    _Data.clear();
  }

  IndexBuffer::IndexBuffer(int num_element, int size)
  {
    Allocate(num_element, size);
  }

  //! Copy constructor
  IndexBuffer::IndexBuffer(const IndexBuffer &Other)
  {
    _Data = Other._Data;
    _Stride = Other._Stride;
  }

  //! Assignment constructor
  IndexBuffer &IndexBuffer::operator = (const IndexBuffer &Other)
  {
    _Data = Other._Data;
    _Stride = Other._Stride;
    return *this;
  }

  void IndexBuffer::Allocate(int num_index, int size)
  {
    nuxAssert(num_index > 0);
    nuxAssert(size > 0);
    nuxAssert((size == 2) || (size == 4));

    if((size <= 0) || (num_index <= 0))
    {
      _Data.clear();
      _Stride = 0;
      return;
    }

    _num_index = num_index;
    // Make sure Size is a multiple of Stride
    int sz = _num_index * size;
    _Stride = size;

    _Data.clear();
    _Data.resize(sz);
  }

  int IndexBuffer::GetSize() const
  {
    return _Data.size();
  }

  int IndexBuffer::GetStride() const
  {
    return _Stride;
  }

  int IndexBuffer::GetNumIndex() const
  {
    return _num_index;
  }

  const void* IndexBuffer::GetPtrRawData() const
  {
    return NUX_STATIC_CAST (const void*, &_Data[0]);
  }

  void* IndexBuffer::GetPtrRawData()
  {
    return NUX_CONST_CAST (void*, ((NUX_CONST_CAST (const IndexBuffer *, this) )->GetPtrRawData()));
  }

  VertexDeclaration::VertexDeclaration()
  {
    _declaration.clear();
  }

  VertexDeclaration::~VertexDeclaration()
  {
    _declaration.clear();
  }

  void VertexDeclaration::AddVertexComponent(VERTEXELEMENT Component)
  {
    _declaration.push_back(Component);
  }


//   NMeshComponent::NMeshComponent()
//     :   _Type (ATTRIB_DECLTYPE_UNKNOWN)
//     ,   _StreamIndex (-1)
//     ,   _Offset (0)
//   {
//   }
// 
//   NMeshComponent::NMeshComponent (const NMeshComponent &Other)
//     :   _Type (ATTRIB_DECLTYPE_UNKNOWN)
//     ,   _StreamIndex (-1)
//     ,   _Offset (0)
//   {
//     _Type = Other._Type;
//     _Offset = Other._Offset;
//     _StreamIndex = Other._StreamIndex;
//   }
// 
//   NMeshComponent::NMeshComponent (int StreamIndex/*ObjectPtr<VertexBuffer> VtxBuffer*/, int Offset, ATTRIB_DECL_TYPE Type)
//   {
//     nuxAssert (Offset >= 0);
//     _Type = Type;
//     _Offset = Offset;
//     _StreamIndex = StreamIndex;
//   }
// 
//   NMeshComponent::~NMeshComponent()
//   {
//   }
// 
//   NMeshComponent &NMeshComponent::operator = (const NMeshComponent &Other)
//   {
//     _Type = Other._Type;
//     _Offset = Other._Offset;
//     _StreamIndex = Other._StreamIndex;
//     return *this;
//   }
// 
//   int NMeshComponent::GetStreamIndex() const
//   {
//     return _StreamIndex;
//   }
// 
// // int NMeshComponent::GetStride()
// // {
// //     return _VtxBuffer->GetStride();
// // }
// 
//   int NMeshComponent::GetOffset()
//   {
//     return _Offset;
//   }

  CachedVertexBuffer::CachedVertexBuffer (NResourceSet *ResourceManager, VertexBuffer *SourceVtxBuffer)
    :   CachedResourceData (ResourceManager)
    ,   _Size (0)
    ,   _Stride(0)
  {
    UpdateResource(SourceVtxBuffer);
  }

  CachedVertexBuffer::~CachedVertexBuffer()
  {
    // Not necessary for a smart pointer but do it anyway to be clear;
    _vertex_buffer = ObjectPtr<IOpenGLVertexBuffer> (0);
  }

  bool CachedVertexBuffer::UpdateResource(ResourceData *Source)
  {
    if(Source == 0)
    {
      _Stride = 0;
      _vertex_buffer.Release();
      return true;
    }

    VertexBuffer *SourceVtxBuffer = 0;
    nuxAssert(Source->Type().IsDerivedFromType(VertexBuffer::StaticObjectType));

    if(Source->Type().IsDerivedFromType(VertexBuffer::StaticObjectType))
    {
      SourceVtxBuffer = NUX_STATIC_CAST(VertexBuffer *, Source);
    }
    else
    {
      return false;
    }

    if((SourceVtxBuffer->GetSize() == 0) || (SourceVtxBuffer->GetStride() == 0))
    {
      _Stride = 0;
      _vertex_buffer.Release();
      return true;
    }

    if(_Size != SourceVtxBuffer->GetSize())
    {
      // The current size of the cached buffer is not the same as the requested one.
      // Delete the previously allocated buffer and create a new one.
      _Size = SourceVtxBuffer->GetSize();
      _Stride = SourceVtxBuffer->GetStride();

      //Release the previous vertex buffer if any.
      _vertex_buffer.Release();
      _vertex_buffer = GetGraphicsDisplay()->GetGpuDevice()->CreateVertexBuffer(_Size, VBO_USAGE_DYNAMIC);
      LoadVertexData(SourceVtxBuffer);
    }
    else
    {
      // The buffer size has changed. Delete the previously allocated buffer and create a new one.
      _Stride = SourceVtxBuffer->GetStride();
      LoadVertexData(SourceVtxBuffer);
    }

    return true;
  }

  void CachedVertexBuffer::LoadVertexData(VertexBuffer *SourceVtxBuffer)
  {
    if((SourceVtxBuffer == 0) || (SourceVtxBuffer->GetSize() == 0) || (SourceVtxBuffer->GetStride() == 0))
    {
      return;
    }

    t_byte *pData;

    _vertex_buffer->Lock(0, 0, (void **) &pData);
    Memcpy(pData, SourceVtxBuffer->GetPtrRawData(), SourceVtxBuffer->GetSize());
    _vertex_buffer->Unlock();
  }

  int CachedVertexBuffer::GetElementSize() const
  {
    return _Size;
  }

  int CachedVertexBuffer::GetBufferStride() const
  {
    return _Stride;
  }

  CachedIndexBuffer::CachedIndexBuffer (NResourceSet *ResourceManager, IndexBuffer *SourceIdxBuffer)
    :   CachedResourceData (ResourceManager)
    ,   _Size(0)
    ,   _Stride(0)
  {
    UpdateResource(SourceIdxBuffer);
  }

  CachedIndexBuffer::~CachedIndexBuffer()
  {
    // Not necessary for a smart pointer but do it anyway to be clear;
    _index_buffer = ObjectPtr<IOpenGLIndexBuffer> (0);
  }

  bool CachedIndexBuffer::UpdateResource (ResourceData *Source)
  {
    if (Source == 0)
    {
      _Size = 0;
      _Stride = 0;
      _index_buffer.Release();
      return true;
    }

    IndexBuffer *SourceIdxBuffer = 0;
    nuxAssert (Source->Type().IsDerivedFromType (IndexBuffer::StaticObjectType) );

    if (Source->Type().IsDerivedFromType (IndexBuffer::StaticObjectType) )
    {
      SourceIdxBuffer = NUX_STATIC_CAST (IndexBuffer *, Source);
    }
    else
    {
      return false;
    }

    if ( (SourceIdxBuffer->GetSize() == 0) || (SourceIdxBuffer->GetStride() == 0) )
    {
      _Size = 0;
      _Stride = 0;
      _index_buffer.Release();
      return true;
    }

    if (_Size != SourceIdxBuffer->GetSize() )
    {
      _Size = SourceIdxBuffer->GetSize();
      _Stride = SourceIdxBuffer->GetStride();

      //Release the previous vertex buffer if any.
      _index_buffer.Release();
      _index_buffer = GetGraphicsDisplay()->GetGpuDevice()->CreateIndexBuffer (_Size, VBO_USAGE_DYNAMIC,
                    (SourceIdxBuffer->GetStride() == 2) ? INDEX_FORMAT_USHORT : INDEX_FORMAT_UINT);
      LoadIndexData (SourceIdxBuffer);
    }
    else
    {
      _Stride = SourceIdxBuffer->GetStride();
      LoadIndexData (SourceIdxBuffer);
    }

    _num_index = SourceIdxBuffer->GetNumIndex();

    return true;
  }

  void CachedIndexBuffer::LoadIndexData (IndexBuffer *SourceIdxBuffer)
  {
    if ( (SourceIdxBuffer == 0) || (SourceIdxBuffer->GetSize() == 0) || (SourceIdxBuffer->GetStride() == 0) )
    {
      return;
    }

    t_byte *pData;
    _index_buffer->Lock (0, 0, (void **) &pData);
    Memcpy (pData, SourceIdxBuffer->GetPtrRawData(), SourceIdxBuffer->GetSize() );
    _index_buffer->Unlock();
  }

  int CachedIndexBuffer::GetElementSize() const
  {
    return _Size;
  }

  int CachedIndexBuffer::GetBufferStride() const
  {
    return _Stride;
  }

  int CachedIndexBuffer::GetNumIndex() const
  {
    return _num_index;
  }

  CachedVertexDeclaration::CachedVertexDeclaration (NResourceSet *ResourceManager, VertexDeclaration *SourceVertexDeclaration)
    :   CachedResourceData (ResourceManager)
  {
    UpdateResource (SourceVertexDeclaration);
  }

  CachedVertexDeclaration::~CachedVertexDeclaration()
  {
    _declaration = ObjectPtr<IOpenGLVertexDeclaration> (0);
  }

  bool CachedVertexDeclaration::UpdateResource (ResourceData *Source)
  {
    if (Source == 0)
    {
      _declaration = ObjectPtr<IOpenGLVertexDeclaration> (0);
      return true;
    }

    VertexDeclaration *SourceVertexDeclaration = 0;
    nuxAssert (Source->Type().IsDerivedFromType (VertexDeclaration::StaticObjectType) );

    if (Source->Type().IsDerivedFromType (VertexDeclaration::StaticObjectType) )
    {
      SourceVertexDeclaration = NUX_STATIC_CAST (VertexDeclaration *, Source);
    }
    else
    {
      return false;
    }

    if (SourceVertexDeclaration == 0)
    {
      _declaration = ObjectPtr<IOpenGLVertexDeclaration> (0);
      return true;
    }

    _declaration = GetGraphicsDisplay()->GetGpuDevice()->CreateVertexDeclaration (&SourceVertexDeclaration->_declaration[0]);
    return true;
  }

  MeshBuffer::MeshBuffer(NUX_FILE_LINE_DECL)
  {
    _vertex_buffer = 0;
    _index_buffer = 0;
    _vertex_declaration = 0;
  }

  MeshBuffer::~MeshBuffer()
  {
    if(_vertex_buffer)
      _vertex_buffer->UnReference();
    if(_index_buffer)
      _index_buffer->UnReference();
    if(_vertex_declaration)
      _vertex_declaration->UnReference();
  }

  bool MeshBuffer::Update(const MeshData* mesh_data)
  {
    int num_element = mesh_data->_num_element;
    int element_size = mesh_data->_element_size;

    _mesh_primitive_type = mesh_data->_mesh_primitive_type;

    _vertex_buffer = new VertexBuffer();
    _vertex_buffer->Allocate(num_element, element_size);
    memcpy(_vertex_buffer->GetPtrRawData(), mesh_data->_vertex_data, num_element * element_size);

    int num_index = mesh_data->_num_index;
    int index_size = mesh_data->_index_size;

    _index_buffer = new IndexBuffer();
    _index_buffer->Allocate(num_index, index_size);
    memcpy(_index_buffer->GetPtrRawData(), mesh_data->_index_data, num_index * index_size);

    _vertex_declaration = new VertexDeclaration();

    VERTEXELEMENT ve_position(0, 0, ATTRIB_CT_FLOAT, 4,
      ATTRIB_USAGE_DECL_POSITION,
      0, 0);

    VERTEXELEMENT ve_normal(0, 16, ATTRIB_CT_FLOAT, 3,
      ATTRIB_USAGE_DECL_NORMAL,
      0, 0);

    _vertex_declaration->AddVertexComponent(ve_position);
    _vertex_declaration->AddVertexComponent(ve_normal);
    _vertex_declaration->AddVertexComponent(DECL_END);

    return true;
  }

  CachedMeshBuffer::CachedMeshBuffer(NResourceSet* ResourceManager, MeshBuffer* resource)
  : CachedResourceData(ResourceManager)
  {
    UpdateResource(resource);
  }

  CachedMeshBuffer::~CachedMeshBuffer()
  {
  }

  bool CachedMeshBuffer::UpdateResource(ResourceData* source)
  {
    nuxAssert(source->Type().IsDerivedFromType(MeshBuffer::StaticObjectType));

    MeshBuffer* mesh_buffer = NUX_STATIC_CAST(MeshBuffer*, source);

    _cached_vertex_buffer = GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource(mesh_buffer->_vertex_buffer);
    _cached_index_buffer = GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource(mesh_buffer->_index_buffer);
    _cached_vertex_declaration = GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource(mesh_buffer->_vertex_declaration);
    _mesh_primitive_type = mesh_buffer->_mesh_primitive_type;
//       
    return true;
  }


//   NMesh::NMesh()
//   {
// 
//   }
// 
//   NMesh::~NMesh()
//   {
// 
//   }
// 
// 
//   NStaticMesh::NStaticMesh (NMeshObject *Object)
//   {
//     if (Object == 0)
//       return;
//   }
// 
//   NStaticMesh::~NStaticMesh()
//   {
//     delete m_pVertexDeclaration;
//     delete m_pIndex;
// 
//     for (int i = 0; i < (int) m_pVertexStreamArray.size(); i++)
//     {
//       delete m_pVertexStreamArray[i];
//     }
//   }
// 
//   int NStaticMesh::GetNumStreams() const
//   {
//     return (int) m_pVertexStreamArray.size();
//   }
// 
//   NGLStaticMesh::NGLStaticMesh (NResourceSet *ResourceManager, NStaticMesh *StaticMesh)
//     :   CachedResourceData (ResourceManager)
//   {
//     int NumStreams = StaticMesh->GetNumStreams();
// 
//     for (int s = 0; s < NumStreams; s++)
//     {
//       m_VertexBufferArray.push_back (GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource (StaticMesh->m_pVertexStreamArray[s]) );
//     }
// 
//     m_Index = ObjectPtr<CachedIndexBuffer> (GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource (StaticMesh->m_pIndex));
//     m_VertexDeclaration = ObjectPtr<CachedVertexDeclaration> (GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource (StaticMesh->m_pVertexDeclaration));
//   }
// 
//   NGLStaticMesh::~NGLStaticMesh()
//   {
// 
//   }
// 
//   bool NGLStaticMesh::UpdateResource (ResourceData *Source)
//   {
//     if (Source == 0)
//     {
//       int NumStreams = m_VertexBufferArray.size();
// 
//       for (int s = 0; s < NumStreams; s++)
//       {
//         m_VertexBufferArray[s]->UpdateResource (0);
//       }
// 
//       m_Index->UpdateResource (0);
//       m_VertexDeclaration->UpdateResource (0);
//       return true;
//     }
// 
//     NStaticMesh *StaticMesh = 0;
//     nuxAssert (Source->Type().IsDerivedFromType (NStaticMesh::StaticObjectType) );
// 
//     if (Source->Type().IsDerivedFromType (NStaticMesh::StaticObjectType) )
//     {
//       StaticMesh = NUX_STATIC_CAST (NStaticMesh *, Source);
//     }
//     else
//     {
//       return false;
//     }
// 
//     int NumStreams = StaticMesh->GetNumStreams();
// 
//     for (int s = 0; s < NumStreams; s++)
//     {
//       GetGraphicsDisplay()->GetGraphicsEngine()->UpdateResource (StaticMesh->m_pVertexStreamArray[s]);
//     }
// 
//     GetGraphicsDisplay()->GetGraphicsEngine()->UpdateResource (StaticMesh->m_pIndex);
//     GetGraphicsDisplay()->GetGraphicsEngine()->UpdateResource (StaticMesh->m_pVertexDeclaration);
//     return true;
//   }


// static void sWriteFloat(float** Ptr, float a, float b, float c, float d)
// {
//     *Ptr[0] = a; *Ptr[1] = b; *Ptr[2] = c; *Ptr[3] = d;
//     *Ptr += 4;
// }

// NStaticMesh::NStaticMesh()
// {
//
// }
//
//
// NCubeVertexBuffer::NCubeVertexBuffer()
// {
//     VERTEXELEMENT decl [] =
//     {
//         {0/*Stream*/, 0/*Offset*/, ATTRIB_CT_FLOAT/*Type*/, 4/*NumComponent*/, ATTRIB_USAGE_DECL_POSITION/*Usage*/, 0/*UsageIndex*/, 0 /*CgParameter*/},
//         DECL_END
//     };
//     INT i = 0;
//     while(decl[i].Stream != 0xFF)
//     {
//         _DeclarationsArray.push_back(decl[i]);
//         i++;
//     }
//
//
//     //          4+------+7
//     //          /|     /|
//     //         / |    / |        y
//     //        / 5+---/--+6       |
//     //      0+------+3 /         |
//     //       | /    | /          *---x
//     //       |/     |/          /
//     //      1+------+2         /
//     //                        z
//
//     float width_    = 1.0f;
//     float height_   = 1.0f;
//     float depth_    = 1.0f;
//     float w_        = 1.0f;
//
//     Vector4 v0, v1, v2;
//     // Front
//     v0 = Vector4(-width_/2.0f, height_/2.0f, depth_/2.0f, w_);       // 0
//     v1 = Vector4(-width_/2.0f, -height_/2.0f, depth_/2.0f, w_);      // 1
//     v2 = Vector4(width_/2.0f, -height_/2.0f, depth_/2.0f, w_);       // 2
//     m_Vertex.push_back(v0); m_Vertex.push_back(v1); m_Vertex.push_back(v2);
//
//     v0 = Vector4(-width_/2.0f, height_/2.0f, depth_/2.0f, w_);       // 0
//     v1 = Vector4(width_/2.0f, -height_/2.0f, depth_/2.0f, w_);       // 2
//     v2 = Vector4(width_/2.0f, height_/2.0f, depth_/2.0f, w_);        // 3
//     m_Vertex.push_back(v0); m_Vertex.push_back(v1); m_Vertex.push_back(v2);
//
//     // Back
//     v0 = Vector4(width_/2.0f, height_/2.0f, -depth_/2.0f, w_);       // 7
//     v1 = Vector4(width_/2.0f, -height_/2.0f, -depth_/2.0f, w_);      // 6
//     v2 = Vector4(-width_/2.0f, -height_/2.0f, -depth_/2.0f, w_);     // 5
//     m_Vertex.push_back(v0); m_Vertex.push_back(v1); m_Vertex.push_back(v2);
//
//     v0 = Vector4(width_/2.0f, height_/2.0f, -depth_/2.0f, w_);       // 7
//     v1 = Vector4(-width_/2.0f, -height_/2.0f, -depth_/2.0f, w_);     // 5
//     v2 = Vector4(-width_/2.0f, height_/2.0f, -depth_/2.0f, w_);      // 4
//     m_Vertex.push_back(v0); m_Vertex.push_back(v1); m_Vertex.push_back(v2);
//
//     // Top
//     v0 = Vector4(-width_/2.0f, height_/2.0f, -depth_/2.0f, w_);      // 4
//     v1 = Vector4(-width_/2.0f, height_/2.0f, depth_/2.0f, w_);       // 0
//     v2 = Vector4(width_/2.0f, height_/2.0f, depth_/2.0f, w_);        // 3
//     m_Vertex.push_back(v0); m_Vertex.push_back(v1); m_Vertex.push_back(v2);
//
//     v0 = Vector4(-width_/2.0f, height_/2.0f, -depth_/2.0f, w_);      // 4
//     v1 = Vector4(width_/2.0f, height_/2.0f, depth_/2.0f, w_);        // 3
//     v2 = Vector4(width_/2.0f, height_/2.0f, -depth_/2.0f, w_);       // 7
//     m_Vertex.push_back(v0); m_Vertex.push_back(v1); m_Vertex.push_back(v2);
//
//     // Bottom
//     v0 = Vector4(width_/2.0f, -height_/2.0f, -depth_/2.0f, w_);      // 6
//     v1 = Vector4(width_/2.0f, -height_/2.0f, depth_/2.0f, w_);       // 2
//     v2 = Vector4(-width_/2.0f, -height_/2.0f, depth_/2.0f, w_);      // 1
//     m_Vertex.push_back(v0); m_Vertex.push_back(v1); m_Vertex.push_back(v2);
//
//     v0 = Vector4(width_/2.0f, -height_/2.0f, -depth_/2.0f, w_);      // 6
//     v1 = Vector4(-width_/2.0f, -height_/2.0f, depth_/2.0f, w_);      // 1
//     v2 = Vector4(-width_/2.0f, -height_/2.0f, -depth_/2.0f, w_);     // 5
//     m_Vertex.push_back(v0); m_Vertex.push_back(v1); m_Vertex.push_back(v2);
//
//     // Left
//     v0 = Vector4(-width_/2.0f, height_/2.0f, -depth_/2.0f, w_);      // 4
//     v1 = Vector4(-width_/2.0f, -height_/2.0f, -depth_/2.0f, w_);     // 5
//     v2 = Vector4(-width_/2.0f, -height_/2.0f, depth_/2.0f, w_);      // 1
//     m_Vertex.push_back(v0); m_Vertex.push_back(v1); m_Vertex.push_back(v2);
//
//     v0 = Vector4(-width_/2.0f, height_/2.0f, -depth_/2.0f, w_);      // 4
//     v1 = Vector4(-width_/2.0f, -height_/2.0f, depth_/2.0f, w_);      // 1
//     v2 = Vector4(-width_/2.0f, height_/2.0f, depth_/2.0f, w_);       // 0
//     m_Vertex.push_back(v0); m_Vertex.push_back(v1); m_Vertex.push_back(v2);
//
//     // Right
//     v0 = Vector4(width_/2.0f, height_/2.0f, depth_/2.0f, w_);        // 3
//     v1 = Vector4(width_/2.0f, -height_/2.0f, depth_/2.0f, w_);       // 2
//     v2 = Vector4(width_/2.0f, -height_/2.0f, -depth_/2.0f, w_);      // 6
//     m_Vertex.push_back(v0); m_Vertex.push_back(v1); m_Vertex.push_back(v2);
//
//     v0 = Vector4(width_/2.0f, height_/2.0f, depth_/2.0f, w_);        // 3
//     v1 = Vector4(width_/2.0f, -height_/2.0f, -depth_/2.0f, w_);      // 6
//     v2 = Vector4(width_/2.0f, height_/2.0f, -depth_/2.0f, w_);       // 7
//     m_Vertex.push_back(v0); m_Vertex.push_back(v1); m_Vertex.push_back(v2);
//
//     m_Size = (INT)m_Vertex.size() * 4 * 4;  // bytes
//     m_Stride = 0;
// }
//
// NCubeVertexBuffer::~NCubeVertexBuffer()
// {
// }
//
// void NCubeVertexBuffer::GetData(void* Buffer)
// {
//     nuxAssert(Buffer);
//
//     float *Ptr = (float*)Buffer;
//
//     INT i = 0;
//     for(i = 0; i < (INT)m_Vertex.size(); i++)
//     {
//         *Ptr++ = m_Vertex[i].x;
//         *Ptr++ = m_Vertex[i].y;
//         *Ptr++ = m_Vertex[i].z;
//         *Ptr++ = m_Vertex[i].w;
//     }
// }
//
// INT NCubeVertexBuffer::GetSize()
// {
//     return m_Size; // bytes
// }
//
// VERTEXELEMENT NCubeVertexBuffer::GetDeclaration(INT i)
// {
//     if(i < (INT)_DeclarationsArray.size())
//     {
//         return _DeclarationsArray[i];
//     }
//     VERTEXELEMENT decl = DECL_END;
//     return decl;
// }

}

