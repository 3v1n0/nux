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


#include "GLResource.h"
#include "GLResourceManager.h"
#include "GLDeviceObjects.h"
#include "GLDeviceFactory.h"
#include "GLVertexResourceManager.h"

#include "NuxMesh/NMeshObject.h"
#include "OpenGLEngine.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

  NUX_IMPLEMENT_OBJECT_TYPE (NVertexBuffer);
  NUX_IMPLEMENT_OBJECT_TYPE (NIndexBuffer);
  NUX_IMPLEMENT_OBJECT_TYPE (NMesh);
  NUX_IMPLEMENT_OBJECT_TYPE (NStaticMesh);
  NUX_IMPLEMENT_OBJECT_TYPE (NVertexDeclaration);

  NUX_IMPLEMENT_OBJECT_TYPE (NGLVertexBuffer);
  NUX_IMPLEMENT_OBJECT_TYPE (NGLIndexBuffer);
//NUX_IMPLEMENT_OBJECT_TYPE(NGLMesh);
  NUX_IMPLEMENT_OBJECT_TYPE (NGLStaticMesh);
  NUX_IMPLEMENT_OBJECT_TYPE (NGLVertexDeclaration);


  NVertexBuffer::NVertexBuffer()
  {
    _Stride = 0;
  }

  NVertexBuffer::~NVertexBuffer()
  {
    _Data.clear();
  }

  NVertexBuffer::NVertexBuffer (int Size, int Stride)
  {
    Allocate (Size, Stride);
  }

//! Copy constructor
  NVertexBuffer::NVertexBuffer (const NVertexBuffer &Other)
  {
    _Data = Other._Data;
    _Stride = Other._Stride;
  }

//! Assignment constructor
  NVertexBuffer &NVertexBuffer::operator = (const NVertexBuffer &Other)
  {
    _Data = Other._Data;
    _Stride = Other._Stride;
    return *this;
  }

  void NVertexBuffer::Allocate (int Size, int Stride)
  {
    nuxAssert (Size > 0);
    nuxAssert (Stride > 0);

    if ( (Stride <= 0) || (Size <= 0) )
    {
      _Data.clear();
      _Stride = 0;
      return;
    }

    // Make sure Size is a multiple of Stride
    int sz = Align<int> (Size, Stride);
    _Stride = Stride;

    _Data.clear();
    _Data.resize (sz);
  }

  int NVertexBuffer::GetSize() const
  {
    return _Data.size();
  }

  int NVertexBuffer::GetStride() const
  {
    return _Stride;
  }

  int NVertexBuffer::GetNumElement() const
  {
    return _Data.size() / _Stride;
  }

  const t_u8 *NVertexBuffer::GetPtrRawData() const
  {
    if (_Data.size() == 0)
      return 0;

    return NUX_STATIC_CAST (const t_u8 *, &_Data[0]);
  }

  t_u8 *NVertexBuffer::GetPtrRawData()
  {
    if (_Data.size() == 0)
      return 0;

    return NUX_CONST_CAST (t_u8 *, ( (NUX_CONST_CAST (const NVertexBuffer *, this) )->GetPtrRawData() ) );
  }

//////////////////////////////////////////////////////////////////////////
  NIndexBuffer::NIndexBuffer()
  {
    _Stride = 0;
  }

  NIndexBuffer::~NIndexBuffer()
  {
    _Data.clear();
  }

  NIndexBuffer::NIndexBuffer (int Size, int Stride)
  {
    Allocate (Size, Stride);
  }

//! Copy constructor
  NIndexBuffer::NIndexBuffer (const NIndexBuffer &Other)
  {
    _Data = Other._Data;
    _Stride = Other._Stride;
  }

//! Assignment constructor
  NIndexBuffer &NIndexBuffer::operator = (const NIndexBuffer &Other)
  {
    _Data = Other._Data;
    _Stride = Other._Stride;
    return *this;
  }

  void NIndexBuffer::Allocate (int Size, int Stride)
  {
    nuxAssert (Size > 0);
    nuxAssert (Stride > 0);

    if ( (Stride <= 0) || (Size <= 0) )
    {
      _Data.clear();
      _Stride = 0;
      return;
    }

    // Make sure Size is a multiple of Stride
    int sz = Align<int> (Size, Stride);
    _Stride = Stride;

    _Data.clear();
    _Data.resize (sz);
  }

  int NIndexBuffer::GetSize() const
  {
    return _Data.size();
  }

  int NIndexBuffer::GetStride() const
  {
    return _Stride;
  }

  int NIndexBuffer::GetNumElement() const
  {
    return _Data.size() / _Stride;
  }

  const t_u8 *NIndexBuffer::GetPtrRawData() const
  {
    return NUX_STATIC_CAST (const t_u8 *, &_Data[0]);
  }

  t_u8 *NIndexBuffer::GetPtrRawData()
  {
    return NUX_CONST_CAST (t_u8 *, ( (NUX_CONST_CAST (const NIndexBuffer *, this) )->GetPtrRawData() ) );
  }

//////////////////////////////////////////////////////////////////////////
  NVertexDeclaration::NVertexDeclaration()
  {
    m_Declaration.clear();
  }

  NVertexDeclaration::~NVertexDeclaration()
  {
    m_Declaration.clear();
  }

  void NVertexDeclaration::AddVertexComponent (VERTEXELEMENT Component)
  {
    m_Declaration.push_back (Component);
  }
//////////////////////////////////////////////////////////////////////////


  NMeshComponent::NMeshComponent()
    :   _Type (ATTRIB_DECLTYPE_UNKNOWN)
    ,   _StreamIndex (-1)
    ,   _Offset (0)
  {
  }

  NMeshComponent::NMeshComponent (const NMeshComponent &Other)
    :   _Type (ATTRIB_DECLTYPE_UNKNOWN)
    ,   _StreamIndex (-1)
    ,   _Offset (0)
  {
    _Type = Other._Type;
    _Offset = Other._Offset;
    _StreamIndex = Other._StreamIndex;
  }

  NMeshComponent::NMeshComponent (int StreamIndex/*TRefGL<NVertexBuffer> VtxBuffer*/, int Offset, ATTRIB_DECL_TYPE Type)
  {
    nuxAssert (Offset >= 0);
    _Type = Type;
    _Offset = Offset;
    _StreamIndex = StreamIndex;
  }

  NMeshComponent::~NMeshComponent()
  {
  }

  NMeshComponent &NMeshComponent::operator = (const NMeshComponent &Other)
  {
    _Type = Other._Type;
    _Offset = Other._Offset;
    _StreamIndex = Other._StreamIndex;
    return *this;
  }

  int NMeshComponent::GetStreamIndex() const
  {
    return _StreamIndex;
  }

// int NMeshComponent::GetStride()
// {
//     return _VtxBuffer->GetStride();
// }

  int NMeshComponent::GetOffset()
  {
    return _Offset;
  }

//////////////////////////////////////////////////////////////////////////
  NGLVertexBuffer::NGLVertexBuffer (NResourceSet *ResourceManager, NVertexBuffer *SourceVtxBuffer)
    :   NGLResource (ResourceManager)
    ,   _Size (0)
    ,   _Stride (0)
  {
    UpdateResource (SourceVtxBuffer);
  }

  NGLVertexBuffer::~NGLVertexBuffer()
  {
    // Not necessary for a smart pointer but do it anyway to be clear;
    m_VtxBuffer = 0;
  }

  bool NGLVertexBuffer::UpdateResource (NResource *Source)
  {
    if (Source == 0)
    {
      _Stride = 0;
      m_VtxBuffer.Release();
      return true;
    }

    NVertexBuffer *SourceVtxBuffer = 0;
    nuxAssert (Source->Type().IsDerivedFromType (NVertexBuffer::StaticObjectType) );

    if (Source->Type().IsDerivedFromType (NVertexBuffer::StaticObjectType) )
    {
      SourceVtxBuffer = NUX_STATIC_CAST (NVertexBuffer *, Source);
    }
    else
    {
      return false;
    }

    if ( (SourceVtxBuffer->GetSize() == 0) || (SourceVtxBuffer->GetStride() == 0) )
    {
      _Stride = 0;
      m_VtxBuffer.Release();
      return true;
    }

    if (_Size != SourceVtxBuffer->GetSize() )
    {
      _Size = SourceVtxBuffer->GetSize();
      _Stride = SourceVtxBuffer->GetStride();

      //Release the previous vertex buffer if any.
      m_VtxBuffer.Release();
      m_VtxBuffer = GetThreadGLDeviceFactory()->CreateVertexBuffer (_Size, VBO_USAGE_DYNAMIC);
      LoadVertexData (SourceVtxBuffer);
    }
    else
    {
      _Stride = SourceVtxBuffer->GetStride();
      LoadVertexData (SourceVtxBuffer);
    }

    return true;
  }

  void NGLVertexBuffer::LoadVertexData (NVertexBuffer *SourceVtxBuffer)
  {
    if ( (SourceVtxBuffer == 0) || (SourceVtxBuffer->GetSize() == 0) || (SourceVtxBuffer->GetStride() == 0) )
    {
      return;
    }

    t_byte *pData;

    m_VtxBuffer->Lock (0, 0, (void **) &pData);
    Memcpy (pData, SourceVtxBuffer->GetPtrRawData(), SourceVtxBuffer->GetSize() );
    m_VtxBuffer->Unlock();
  }

//////////////////////////////////////////////////////////////////////////
  NGLIndexBuffer::NGLIndexBuffer (NResourceSet *ResourceManager, NIndexBuffer *SourceIdxBuffer)
    :   NGLResource (ResourceManager)
    ,   _Size (0)
    ,   _Stride (0)
  {
    UpdateResource (SourceIdxBuffer);
  }

  NGLIndexBuffer::~NGLIndexBuffer()
  {
    // Not necessary for a smart pointer but do it anyway to be clear;
    m_IdxBuffer = 0;
  }

  bool NGLIndexBuffer::UpdateResource (NResource *Source)
  {
    if (Source == 0)
    {
      _Size = 0;
      _Stride = 0;
      m_IdxBuffer.Release();
      return true;
    }

    NIndexBuffer *SourceIdxBuffer = 0;
    nuxAssert (Source->Type().IsDerivedFromType (NIndexBuffer::StaticObjectType) );

    if (Source->Type().IsDerivedFromType (NIndexBuffer::StaticObjectType) )
    {
      SourceIdxBuffer = NUX_STATIC_CAST (NIndexBuffer *, Source);
    }
    else
    {
      return false;
    }

    if ( (SourceIdxBuffer->GetSize() == 0) || (SourceIdxBuffer->GetStride() == 0) )
    {
      _Size = 0;
      _Stride = 0;
      m_IdxBuffer.Release();
      return true;
    }

    if (_Size != SourceIdxBuffer->GetSize() )
    {
      _Size = SourceIdxBuffer->GetSize();
      _Stride = SourceIdxBuffer->GetStride();

      //Release the previous vertex buffer if any.
      m_IdxBuffer.Release();
      m_IdxBuffer = GetThreadGLDeviceFactory()->CreateIndexBuffer (_Size, VBO_USAGE_DYNAMIC,
                    (SourceIdxBuffer->GetStride() == 2) ? INDEX_FORMAT_USHORT : INDEX_FORMAT_UINT);
      LoadIndexData (SourceIdxBuffer);
    }
    else
    {
      _Stride = SourceIdxBuffer->GetStride();
      LoadIndexData (SourceIdxBuffer);
    }

    return true;
  }

  void NGLIndexBuffer::LoadIndexData (NIndexBuffer *SourceIdxBuffer)
  {
    if ( (SourceIdxBuffer == 0) || (SourceIdxBuffer->GetSize() == 0) || (SourceIdxBuffer->GetStride() == 0) )
    {
      return;
    }

    t_byte *pData;
    m_IdxBuffer->Lock (0, 0, (void **) &pData);
    Memcpy (pData, SourceIdxBuffer->GetPtrRawData(), SourceIdxBuffer->GetSize() );
    m_IdxBuffer->Unlock();
  }

  NGLVertexDeclaration::NGLVertexDeclaration (NResourceSet *ResourceManager, NVertexDeclaration *SourceVertexDeclaration)
    :   NGLResource (ResourceManager)
  {
    UpdateResource (SourceVertexDeclaration);
  }

  NGLVertexDeclaration::~NGLVertexDeclaration()
  {
    m_VtxDeclaration = 0;
  }

  bool NGLVertexDeclaration::UpdateResource (NResource *Source)
  {
    if (Source == 0)
    {
      m_VtxDeclaration = 0;
      return true;
    }

    NVertexDeclaration *SourceVertexDeclaration = 0;
    nuxAssert (Source->Type().IsDerivedFromType (NVertexDeclaration::StaticObjectType) );

    if (Source->Type().IsDerivedFromType (NVertexDeclaration::StaticObjectType) )
    {
      SourceVertexDeclaration = NUX_STATIC_CAST (NVertexDeclaration *, Source);
    }
    else
    {
      return false;
    }

    if (SourceVertexDeclaration == 0)
    {
      m_VtxDeclaration = 0;
      return true;
    }

    m_VtxDeclaration = GetThreadGLDeviceFactory()->CreateVertexDeclaration (&SourceVertexDeclaration->m_Declaration[0]);
    return true;
  }

  NMesh::NMesh()
  {

  }

  NMesh::~NMesh()
  {

  }


  NStaticMesh::NStaticMesh (NMeshObject *Object)
  {
    if (Object == 0)
      return;

    int hasNormal = 0;
    int hasColor = 0;
    int hasTextureCoord = 0;
    int hasBinormal = 0;
    int hasTangent = 0;

    int OffsetToNormal = 0;
    int OffsetToColor = 0;
    int OffsetToTextureCoord = 0;
    int OffsetToBinormal = 0;
    int OffsetToTangent = 0;

    int buffersize = Object->GetVertexBufferSize();
    int vtxsize = sizeof (Vec4<float>);

    if (Object->HasNormal() )
    {
      hasNormal = 1;
      OffsetToNormal = vtxsize;
      buffersize += Object->GetNormalBufferSize();
      vtxsize += sizeof (Vec3<float>);
    }

    if (Object->HasColor() )
    {
      hasColor = 1;
      OffsetToColor = vtxsize;
      buffersize += Object->GetColorBufferSize();
      vtxsize += sizeof (Vec4<float>);
    }

    if (Object->HasBinormal() )
    {
      hasBinormal = 1;
      OffsetToBinormal = vtxsize;
      buffersize += Object->GetNormalBufferSize();
      vtxsize += sizeof (Vec3<float>);
    }

    if (Object->HasTangent() )
    {
      hasTangent = 1;
      OffsetToTangent = vtxsize;
      buffersize += Object->GetNormalBufferSize();
      vtxsize += sizeof (Vec3<float>);
    }

    if (Object->HasTexCoord() )
    {
      hasTextureCoord = 1;
      OffsetToTextureCoord = vtxsize;
      buffersize += Object->GetTextureBufferSize();
      vtxsize += sizeof (Vec2<float>);
    }

    NVertexBuffer *vb = new NVertexBuffer (buffersize, vtxsize);

    t_byte *data = vb->GetPtrRawData();

    for (unsigned int i = 0; i < Object->GetNumPolygon(); i++)
    {
      Memcpy (data, & (Object->m_VertexArray[i]), sizeof (Vec4<float>) );

      if (hasNormal)
      {
        Memcpy (data + OffsetToNormal, & (Object->m_NormalArray[0][i]), sizeof (Vec3<float>) );
      }

      if (hasBinormal)
      {
        Memcpy (data + OffsetToBinormal, & (Object->m_BinormalArrayXSI[0][i]), sizeof (Vec3<float>) );
      }

      if (hasTangent)
      {
        Memcpy (data + OffsetToTangent, & (Object->m_TangentArrayXSI[0][i]), sizeof (Vec3<float>) );
      }

      if (hasColor)
      {
        Memcpy (data + OffsetToColor, & (Object->m_ColorArray[0][i]), sizeof (Vec4<float>) );
      }

      if (hasTextureCoord)
      {
        Memcpy (data + OffsetToTextureCoord, & (Object->m_TextureArray[0][i]), sizeof (Vec2<float>) );
      }

      data += vtxsize;
    }

    m_pVertexStreamArray.push_back (vb);

    {
      m_pVertexDeclaration = new NVertexDeclaration();
      // Position
      m_pVertexDeclaration->AddVertexComponent (VERTEXELEMENT (0, 0, ATTRIB_CT_FLOAT, 4, ATTRIB_USAGE_DECL_POSITION, 0, 0) );

      if (hasNormal)
      {
        m_pVertexDeclaration->AddVertexComponent (VERTEXELEMENT (0, OffsetToNormal, ATTRIB_CT_FLOAT, 3, ATTRIB_USAGE_DECL_NORMAL, 0, 0) );
      }

      if (hasBinormal)
      {
        m_pVertexDeclaration->AddVertexComponent (VERTEXELEMENT (0, OffsetToBinormal, ATTRIB_CT_FLOAT, 3, ATTRIB_USAGE_DECL_BINORMAL, 0, 0) );
      }

      if (hasTangent)
      {
        m_pVertexDeclaration->AddVertexComponent (VERTEXELEMENT (0, OffsetToTangent, ATTRIB_CT_FLOAT, 3, ATTRIB_USAGE_DECL_TANGENT, 0, 0) );
      }

      if (hasColor)
      {
        m_pVertexDeclaration->AddVertexComponent (VERTEXELEMENT (0, OffsetToColor, ATTRIB_CT_FLOAT, 4, ATTRIB_USAGE_DECL_COLOR, 0, 0) );
      }

      if (hasTextureCoord)
      {
        m_pVertexDeclaration->AddVertexComponent (VERTEXELEMENT (0, OffsetToTextureCoord, ATTRIB_CT_FLOAT, 2, ATTRIB_USAGE_DECL_TEXCOORD, 0, 0) );
      }

      m_pVertexDeclaration->AddVertexComponent (DECL_END);
    }



    NIndexBuffer *ib    = new NIndexBuffer (Object->m_IndexArray.size(), 4);
    Memcpy (ib->GetPtrRawData(), &Object->m_IndexArray[0], Object->m_IndexArray.size() );
    m_pIndex             = ib;
  }

  NStaticMesh::~NStaticMesh()
  {
    delete m_pVertexDeclaration;
    delete m_pIndex;

    for (int i = 0; i < (int) m_pVertexStreamArray.size(); i++)
    {
      delete m_pVertexStreamArray[i];
    }
  }

  int NStaticMesh::GetNumStreams() const
  {
    return (int) m_pVertexStreamArray.size();
  }

  NGLStaticMesh::NGLStaticMesh (NResourceSet *ResourceManager, NStaticMesh *StaticMesh)
    :   NGLResource (ResourceManager)
  {
    int NumStreams = StaticMesh->GetNumStreams();

    for (int s = 0; s < NumStreams; s++)
    {
      m_VertexBufferArray.push_back (GetThreadGraphicsContext()->CacheResource (StaticMesh->m_pVertexStreamArray[s]) );
    }

    m_Index = GetThreadGraphicsContext()->CacheResource (StaticMesh->m_pIndex);
    m_VertexDeclaration = GetThreadGraphicsContext()->CacheResource (StaticMesh->m_pVertexDeclaration);
  }

  NGLStaticMesh::~NGLStaticMesh()
  {

  }

  bool NGLStaticMesh::UpdateResource (NResource *Source)
  {
    if (Source == 0)
    {
      int NumStreams = m_VertexBufferArray.size();

      for (int s = 0; s < NumStreams; s++)
      {
        m_VertexBufferArray[s]->UpdateResource (0);
      }

      m_Index->UpdateResource (0);
      m_VertexDeclaration->UpdateResource (0);
      return true;
    }

    NStaticMesh *StaticMesh = 0;
    nuxAssert (Source->Type().IsDerivedFromType (NStaticMesh::StaticObjectType) );

    if (Source->Type().IsDerivedFromType (NStaticMesh::StaticObjectType) )
    {
      StaticMesh = NUX_STATIC_CAST (NStaticMesh *, Source);
    }
    else
    {
      return false;
    }

    int NumStreams = StaticMesh->GetNumStreams();

    for (int s = 0; s < NumStreams; s++)
    {
      GetThreadGraphicsContext()->UpdateResource (StaticMesh->m_pVertexStreamArray[s]);
    }

    GetThreadGraphicsContext()->UpdateResource (StaticMesh->m_pIndex);
    GetThreadGraphicsContext()->UpdateResource (StaticMesh->m_pVertexDeclaration);
    return true;
  }


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

} //NUX_NAMESPACE_END

