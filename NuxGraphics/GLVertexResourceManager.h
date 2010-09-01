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


#ifndef GLVERTEXRESOURCEMANAGER_H
#define GLVERTEXRESOURCEMANAGER_H

#include "GLResourceManager.h"
#include "IOpenGLVertexBuffer.h"
#include "IOpenGLIndexBuffer.h"
#include "IOpenGLVertexDeclaration.h"
#include "NuxMesh/NMeshObject.h"

NAMESPACE_BEGIN_OGL

class NMeshObject;

class NVertexBuffer: public NResource
{
    DECLARE_OBJECT_TYPE(NVertexBuffer, NResource);

public:
    NVertexBuffer();
    virtual ~NVertexBuffer();
    NVertexBuffer(int Size, int Stride);
    //! Copy constructor
    NVertexBuffer(const NVertexBuffer&);
    //! Assignment constructor
    NVertexBuffer& operator = (const NVertexBuffer&);

    void Allocate(int Size, int Stride);
    int GetSize() const;
    int GetStride() const;
    int GetNumElement() const;
    const t_u8* GetPtrRawData() const;
    t_u8* GetPtrRawData();

public:
    std::vector<t_u8> _Data;
    int _Stride;
};

class NIndexBuffer: public NResource
{
    DECLARE_OBJECT_TYPE(NIndexBuffer, NResource);

    // NIndexBuffer is very similar to NVertexBuffer except that vertex indices
    // are not inter-mixed with other vertex data. So the Stride here should be 2 bytes or 4 bytes.
public:
    NIndexBuffer();
    virtual ~NIndexBuffer();
    NIndexBuffer(int Size, int Stride);
    //! Copy constructor
    NIndexBuffer(const NIndexBuffer&);
    //! Assignment constructor
    NIndexBuffer& operator = (const NIndexBuffer&);

    void Allocate(int Size, int Stride);
    int GetSize() const;
    int GetStride() const;
    int GetNumElement() const;
    const t_u8* GetPtrRawData() const;
    t_u8* GetPtrRawData();

public:
    std::vector<t_u8> _Data;
    int _Stride;
};

class NVertexDeclaration: public NResource
{
    DECLARE_OBJECT_TYPE(NVertexDeclaration, NResource);

public:
    NVertexDeclaration();
    ~NVertexDeclaration();
    void AddVertexComponent(VERTEXELEMENT);
    std::vector<VERTEXELEMENT> m_Declaration;
};

class NGLVertexBuffer: public NGLResource
{
    DECLARE_OBJECT_TYPE(NGLVertexBuffer, NGLResource);

public:
    NGLVertexBuffer(NResourceSet* ResourceManager, NVertexBuffer* VertexData);
    ~NGLVertexBuffer();

    TRefGL<IOpenGLVertexBuffer>	m_VtxBuffer;

    bool UpdateResource(NResource* Resource);
private:
    void LoadVertexData(NVertexBuffer* VertexData);
    int _Size;
    int _Stride;
};

class NGLIndexBuffer: public NGLResource
{
    DECLARE_OBJECT_TYPE(NGLIndexBuffer, NGLResource);
public:
    NGLIndexBuffer(NResourceSet* ResourceManager, NIndexBuffer* Resource); 
    ~NGLIndexBuffer();

    TRefGL<IOpenGLIndexBuffer>	m_IdxBuffer;

    bool UpdateResource(NResource* Resource);
private:
    void LoadIndexData(NIndexBuffer* VertexData);
    int _Size;
    int _Stride;
};

class NGLVertexDeclaration: public NGLResource
{
    DECLARE_OBJECT_TYPE(NGLVertexDeclaration, NGLResource);
public:
    NGLVertexDeclaration(NResourceSet* ResourceManager, NVertexDeclaration* Resource);
    ~NGLVertexDeclaration();
    
    bool UpdateResource(NResource* Resource);
    TRefGL< IOpenGLVertexDeclaration > m_VtxDeclaration;
};

class NMeshComponent
{
public:
    NMeshComponent();
    NMeshComponent(const NMeshComponent&);
    NMeshComponent(int StreamIndex/*TRefGL<NVertexBuffer> VtxBuffer*/, int Offset, ATTRIB_DECL_TYPE Type);

    NMeshComponent& operator = (const NMeshComponent&);

    virtual ~NMeshComponent();

    int GetStreamIndex() const;
//     int GetStride();
    int GetOffset();
private:
    ATTRIB_DECL_TYPE _Type;
    int _StreamIndex;
    int _Offset;
};

class NMesh: public NResource
{
    DECLARE_OBJECT_TYPE(NStaticMesh, NResource);

    NMesh();
    virtual ~NMesh();
};

class NStaticMesh: public NMesh
{
    DECLARE_OBJECT_TYPE(NStaticMesh, NMesh);
public:
    NStaticMesh(NMeshObject* Object);
    ~NStaticMesh();

    int GetNumStreams() const;

    std::vector< NVertexBuffer* > m_pVertexStreamArray;  // up to 8 stream of buffers on most GPU
    NIndexBuffer* m_pIndex;
    NVertexDeclaration* m_pVertexDeclaration;
};

class NGLStaticMesh: public NGLResource
{
    DECLARE_OBJECT_TYPE(NGLStaticMesh, NGLResource);
public:
    NGLStaticMesh(NResourceSet* ResourceManager, NStaticMesh*);
    ~NGLStaticMesh();
    bool UpdateResource(NResource* Resource);

    std::vector< TRefGL< NGLVertexBuffer > > m_VertexBufferArray;
    TRefGL< NGLIndexBuffer > m_Index;
    TRefGL< NGLVertexDeclaration > m_VertexDeclaration;
};

NAMESPACE_END_OGL

#endif // GLVERTEXRESOURCEMANAGER_H
