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


#include "NuxCore/NKernel.h"
#include "NuxCore/Math/Constants.h"
#include "NuxCore/Math/Vector3.h"
#include "NuxCore/Math/Matrix4.h"

#include "GLRenderableMesh.h"

#define CHECKGLX(a) a

#if defined(NUX_OS_WINDOWS)
    extern GLEWContext* glewGetContext();
    extern WGLEWContext* wglewGetContext();
#elif defined(NUX_OS_LINUX)
    extern GLEWContext* glewGetContext();
    extern GLXEWContext* glxewGetContext();
#endif

GLRenderableMesh::GLRenderableMesh()
{
    m_hasNormals    = false;
    m_hasColors     = false;
    m_hasTexCoords  = false;
    m_hasTangentSpace = false;
    m_TexCoordCount = 0;

    m_GLVBOVertex = 0;
    m_GLVBOColor = 0;
    m_GLVBONormal = 0;
    m_GLVBOBiNormal = 0;
    m_GLVBOTangent = 0;

    m_GLVBOTexture0 = 0;
    m_GLVBOTexture1 = 0;
    m_GLVBOTexture2 = 0;
    m_GLVBOTexture3 = 0;
    m_GLVBOTexture4 = 0;
    m_GLVBOTexture5 = 0;
    m_GLVBOTexture6 = 0;
    m_GLVBOTexture7 = 0;
    m_GLVBOTexture8 = 0;
    m_GLVBOTexture9 = 0;
    m_GLVBOTexture10 = 0;
    m_GLVBOTexture11 = 0;
    m_GLVBOTexture12 = 0;
    m_GLVBOTexture13 = 0;
    m_GLVBOTexture14 = 0;
    m_GLVBOTexture15 = 0;

    m_GLVBOIndexVertex = 0;

    m_translation            = nux::Vector3(0.0f, 0.0f, 0.0f);
    m_rotation               = nux::Vector3(0.0f, 0.0f, 0.0f);
    m_scaling                = nux::Vector3(1.0f, 1.0f, 1.0f);


//    m_VertexArray   = 0;
//    m_ColorArray    = 0;
//    m_NormalArray   = 0;
//    m_TangentArray  = 0;
//    m_BinormalArray = 0; 
//    m_UVArray       = 0;
//    m_IndexArray    = 0;
//    m_SubMeshTriangleCount = 0;
}

GLRenderableMesh::~GLRenderableMesh()
{
    
}

void GLRenderableMesh::LoadXSI(const TCHAR* filename)
{
    MeshData2* meshdata = new MeshData2;
    //LoadDotXSI(meshdata, TCHAR_TO_ANSI(filename));

    CreateGLArrayData(meshdata->m_ChildArray[0]);
    LoadVBO();

    delete meshdata;
}

void GLRenderableMesh::Unload()
{
// Use SmartPointer instead

//    for(int i = 0; i < m_SubMeshCount; i++)
//    {
//        delete m_VertexArray[i];
//        delete m_IndexArray[i];
//        if(m_hasColors)
//            delete m_ColorArray[i];
//        if(m_hasNormals)
//            delete m_NormalArray[i];
//        if(m_hasTangentSpace)
//        {
//            delete m_TangentArray[i];
//            delete m_BinormalArray[i];
//        }
//    }
//    delete m_VertexArray;
//    delete m_IndexArray;
//    if(m_hasColors)
//        delete m_ColorArray;
//    if(m_hasNormals)
//        delete m_NormalArray;
//    if(m_hasTangentSpace)
//    {
//        delete m_TangentArray;
//        delete m_BinormalArray;
//    }
//
//
//    for(int j = 0; j < m_SubMeshCount; j++)
//    {
//        for(int k = 0; k < m_TexCoordCount; k++)
//        {
//            delete m_UVArray[j][k];
//        }
//        delete m_UVArray[j];
//    }
//    delete m_VertexArray;
}

bool GLRenderableMesh::CreateGLArrayData(MeshData2* MeshData)
{
    MeshData2* md = MeshData;
    int nbPrimitive = md->m_SubMeshCount;

    m_VertexArray = new nux::Vec4<float>* [nbPrimitive]; 
    m_IndexArray = new int* [nbPrimitive];
    m_SubMeshTriangleCount = new int [nbPrimitive];
    m_SubMeshCount = nbPrimitive;
    m_ArrayStats = new GLRenderableMesh::ArrayStats[nbPrimitive];
    //md->m_SubMeshTriangleCount[0] = 100;

    for(int Primitive = 0; Primitive < nbPrimitive; Primitive++)
    {
        m_VertexArray[Primitive] = new nux::Vec4<float> [md->m_SubMeshTriangleCount[Primitive]*3];

        int nbTriangle = md->m_SubMeshTriangleCount[Primitive];
        int count = 0;
        nux::Vec4<float> v0,v1,v2;
        for(long j = 0; j < nbTriangle; j++)
        {
            v0 = md->VertexPool[md->VertexIndexArray[Primitive][j].i];
            v1 = md->VertexPool[md->VertexIndexArray[Primitive][j].j];
            v2 = md->VertexPool[md->VertexIndexArray[Primitive][j].k];

//            v0.x *= 0.001; v0.y *= 0.001; v0.z *= 0.001; 
//            v1.x *= 0.001; v1.y *= 0.001; v1.z *= 0.001; 
//            v2.x *= 0.001; v2.y *= 0.001; v2.z *= 0.001; 
            m_VertexArray[Primitive][count++] = v0;
            m_VertexArray[Primitive][count++] = v1;
            m_VertexArray[Primitive][count++] = v2;
        }
        m_SubMeshTriangleCount[Primitive] = nbTriangle;
        m_IndexArray[Primitive] = new int[nbTriangle*3];

        // After that the VertexArray for primitive #Primitive is like:
        // [Triangle0<v0,v1,v2>, Triangle1<v0,v1,v2> .... TriangleN<v0,v1,v2>]
        // The index array is:
        // [Triangle0<0,1,2>, Triangle1<3,4,5>, ..... TriangleN<3N,3N+1,3N+2>] 
        int nbIndex = nbTriangle * 3;
        
        m_ArrayStats[Primitive].MaxIndex = 0;
        m_ArrayStats[Primitive].MinIndex = 0xFFFFFFFF;
        m_ArrayStats[Primitive].nbTriangles = nbTriangle;
        for(int index = 0; index < nbIndex; index++)
        {
            m_IndexArray[Primitive][index] = index;
            if(m_ArrayStats[Primitive].MaxIndex < index)
            {
                m_ArrayStats[Primitive].MaxIndex = index;
            }

            if(m_ArrayStats[Primitive].MinIndex > index)
            {
                m_ArrayStats[Primitive].MinIndex = index;
            }
        }
    }

    if(md->nbColor > 0)
    {
        m_hasColors = true;
        m_ColorArray  = new nux::Vec4<float>* [nbPrimitive];
        for(long Primitive = 0; Primitive < nbPrimitive; Primitive++)
        {
            m_ColorArray[Primitive] = new nux::Vec4<float> [md->m_SubMeshTriangleCount[Primitive]*3]; 

            int nbTriangle = md->m_SubMeshTriangleCount[Primitive];
            int count = 0; 
            nux::Vec4<float> v0,v1,v2;
            for(long j = 0; j < nbTriangle; j++)
            {
                v0 = md->ColorPool[md->ColorIndexArray[Primitive][j].i];
                v1 = md->ColorPool[md->ColorIndexArray[Primitive][j].j];
                v2 = md->ColorPool[md->ColorIndexArray[Primitive][j].k];

                m_ColorArray[Primitive][count++] = v0;
                m_ColorArray[Primitive][count++] = v1;
                m_ColorArray[Primitive][count++] = v2;
            }
        }
    }

    if(md->nbNormal > 0)
    {
        m_hasNormals = true;
        m_NormalArray = new nux::Vec3<float>* [nbPrimitive];
        for(long Primitive = 0; Primitive < nbPrimitive; Primitive++)
        {
            m_NormalArray[Primitive] = new nux::Vec3<float> [md->m_SubMeshTriangleCount[Primitive]*3]; 

            int nbTriangle = md->m_SubMeshTriangleCount[Primitive];
            int count = 0;
            nux::Vec3<float> v0,v1,v2;
            for(long j = 0; j < nbTriangle; j++)
            {
                v0 = md->NormalPool[md->NormalIndexArray[Primitive][j].i];
                v1 = md->NormalPool[md->NormalIndexArray[Primitive][j].j];
                v2 = md->NormalPool[md->NormalIndexArray[Primitive][j].k];

                v0.Normalize();
                v1.Normalize();
                v2.Normalize();
                m_NormalArray[Primitive][count++] = v0;
                m_NormalArray[Primitive][count++] = v1;
                m_NormalArray[Primitive][count++] = v2;
            }
        }
    }

    if(md->nbUVSets > 0)
    {
        m_hasTexCoords = true;
        m_TexCoordCount = md->nbUVSets;

        m_UVArray     = new nux::Vec2<float>** [nbPrimitive]; 
        for(long Primitive = 0; Primitive < nbPrimitive; Primitive++)
        {
            m_UVArray[Primitive] = new nux::Vec2<float>* [md->nbUVSets]; 

            for(long j = 0; j < md->nbUVSets; j++)
            {
                m_UVArray[Primitive][j] = new nux::Vec2<float> [md->m_SubMeshTriangleCount[Primitive]*3]; 


                int nbTriangle = md->m_SubMeshTriangleCount[Primitive];
                int count = 0;
                nux::Vec2<float> v0,v1,v2;
                for(long k = 0; k < nbTriangle; k++)
                {
                    v0 = md->UVPool[j][md->UVIndexArray[Primitive][j][k].i];
                    v1 = md->UVPool[j][md->UVIndexArray[Primitive][j][k].j];
                    v2 = md->UVPool[j][md->UVIndexArray[Primitive][j][k].k];

                    m_UVArray[Primitive][j][count++] = v0;
                    m_UVArray[Primitive][j][count++] = v1;
                    m_UVArray[Primitive][j][count++] = v2;
                }   
            }
        }
    }

    if(m_hasNormals && m_hasTexCoords)
    {
        m_TangentArray = new nux::Vec3<float>* [nbPrimitive];
        m_BinormalArray = new nux::Vec3<float>* [nbPrimitive];
        for(long Primitive = 0; Primitive < nbPrimitive; Primitive++)
        {
            m_TangentArray[Primitive] = new nux::Vec3<float> [md->m_SubMeshTriangleCount[Primitive]*3];
            m_BinormalArray[Primitive] = new nux::Vec3<float> [md->m_SubMeshTriangleCount[Primitive]*3];

            CalculateTangentArray(m_VertexArray[Primitive],
                m_NormalArray[Primitive],
                m_UVArray[Primitive][0],
                md->m_SubMeshTriangleCount[Primitive],
                m_IndexArray[Primitive],
                m_TangentArray[Primitive],
                m_BinormalArray[Primitive]);
        }
        m_hasTangentSpace = true;
    }
    return true;
}

void GLRenderableMesh::CreateGLArrayData3(MeshData3* MeshData)
{
    MeshData3* md = MeshData;
    int nbPrimitive = md->m_SubMeshCount;

    m_VertexArray = new nux::Vec4<float>* [nbPrimitive]; 
    m_IndexArray = new int* [nbPrimitive];
    m_SubMeshTriangleCount = new int [nbPrimitive];
    m_SubMeshCount = nbPrimitive;

    for(long Primitive = 0; Primitive < nbPrimitive; Primitive++)
    {
        m_VertexArray[Primitive] = new nux::Vec4<float> [md->m_SubMeshTriangleCount[Primitive]*3];

        int nbTriangle = md->m_SubMeshTriangleCount[Primitive];
        int count = 0;
        nux::Vec4<float> v0,v1,v2;
        for(long j = 0; j < nbTriangle; j++)
        {
            v0 = md->VertexPool[md->VertexIndexArray[Primitive][j].i];
            v1 = md->VertexPool[md->VertexIndexArray[Primitive][j].j];
            v2 = md->VertexPool[md->VertexIndexArray[Primitive][j].k];

            m_VertexArray[Primitive][count++] = v0;
            m_VertexArray[Primitive][count++] = v1;
            m_VertexArray[Primitive][count++] = v2;
        }
        m_SubMeshTriangleCount[Primitive] = nbTriangle;
        m_IndexArray[Primitive] = new int[nbTriangle*3];

        // After that the VertexArray for primitive #Primitive is like:
        // [Triangle0<v0,v1,v2>, Triangle1<v0,v1,v2> .... TriangleN<v0,v1,v2>]
        // The index array is:
        // [Triangle0<0,1,2>, Triangle1<3,4,5>, ..... TriangleN<3N,3N+1,3N+2>] 
        long nbIndex = nbTriangle * 3;
        for(long index = 0; index < nbIndex; index++)
        {
            m_IndexArray[Primitive][index] = index;
        }
    }

    if(md->nbColor > 0)
    {
        m_hasColors = true;
        m_ColorArray  = new nux::Vec4<float>* [nbPrimitive];
        for(long Primitive = 0; Primitive < nbPrimitive; Primitive++)
        {
            m_ColorArray[Primitive] = new nux::Vec4<float> [md->m_SubMeshTriangleCount[Primitive]*3]; 

            int nbTriangle = md->m_SubMeshTriangleCount[Primitive];
            int count = 0; 
            nux::Vec4<float> v0,v1,v2;
            for(long j = 0; j < nbTriangle; j++)
            {
                v0 = md->ColorPool[md->VertexIndexArray[Primitive][j].i];
                v1 = md->ColorPool[md->VertexIndexArray[Primitive][j].j];
                v2 = md->ColorPool[md->VertexIndexArray[Primitive][j].k];

                m_ColorArray[Primitive][count++] = v0;
                m_ColorArray[Primitive][count++] = v1;
                m_ColorArray[Primitive][count++] = v2;
            }
        }
    }

    if(md->nbNormal > 0)
    {
        m_hasNormals = true;
        m_NormalArray = new nux::Vec3<float>* [nbPrimitive];
        for(long Primitive = 0; Primitive < nbPrimitive; Primitive++)
        {
            m_NormalArray[Primitive] = new nux::Vec3<float> [md->m_SubMeshTriangleCount[Primitive]*3]; 

            int nbTriangle = md->m_SubMeshTriangleCount[Primitive];
            int count = 0;
            nux::Vec3<float> v0,v1,v2;
            for(long j = 0; j < nbTriangle; j++)
            {
                v0 = md->NormalPool[md->VertexIndexArray[Primitive][j].i];
                v1 = md->NormalPool[md->VertexIndexArray[Primitive][j].j];
                v2 = md->NormalPool[md->VertexIndexArray[Primitive][j].k];

                v0.Normalize();
                v1.Normalize();
                v2.Normalize();
                m_NormalArray[Primitive][count++] = v0;
                m_NormalArray[Primitive][count++] = v1;
                m_NormalArray[Primitive][count++] = v2;
            }
        }
    }

    if(md->nbUVSets > 0)
    {
        m_hasTexCoords = true;
        m_TexCoordCount = md->nbUVSets;

        m_UVArray     = new nux::Vec2<float>** [nbPrimitive]; 
        for(long Primitive = 0; Primitive < nbPrimitive; Primitive++)
        {
            m_UVArray[Primitive] = new nux::Vec2<float>* [md->nbUVSets]; 

            for(long j = 0; j < md->nbUVSets; j++)
            {
                m_UVArray[Primitive][j] = new nux::Vec2<float> [md->m_SubMeshTriangleCount[Primitive]*3]; 


                int nbTriangle = md->m_SubMeshTriangleCount[Primitive];
                int count = 0;
                nux::Vec3<float> v0,v1,v2;
                for(long k = 0; k < nbTriangle; k++)
                {
//                    int a = md->UVIndexArray[Primitive][j][k].i;
//                    int b = md->UVIndexArray[Primitive][j][k].j;
//                    int c = md->UVIndexArray[Primitive][j][k].k;
                    v0 = md->UVPool[j][md->VertexIndexArray[Primitive][k].i];
                    v1 = md->UVPool[j][md->VertexIndexArray[Primitive][k].j];
                    v2 = md->UVPool[j][md->VertexIndexArray[Primitive][k].k];

                    m_UVArray[Primitive][j][count++] = nux::Vec2<float>(v0.x, v0.y);
                    m_UVArray[Primitive][j][count++] = nux::Vec2<float>(v1.x, v1.y);
                    m_UVArray[Primitive][j][count++] = nux::Vec2<float>(v2.x, v2.y);
                }   
            }
        }
    }

    if(m_hasNormals && m_hasTexCoords)
    {
        m_TangentArray = new nux::Vec3<float>* [nbPrimitive];
        m_BinormalArray = new nux::Vec3<float>* [nbPrimitive];
        for(long Primitive = 0; Primitive < nbPrimitive; Primitive++)
        {
            m_TangentArray[Primitive] = new nux::Vec3<float> [md->m_SubMeshTriangleCount[Primitive]*3];
            m_BinormalArray[Primitive] = new nux::Vec3<float> [md->m_SubMeshTriangleCount[Primitive]*3];

            CalculateTangentArray(m_VertexArray[Primitive],
                m_NormalArray[Primitive],
                m_UVArray[Primitive][0],
                md->m_SubMeshTriangleCount[Primitive],
                m_IndexArray[Primitive],
                m_TangentArray[Primitive],
                m_BinormalArray[Primitive]);
        }
        m_hasTangentSpace = true;
    }


    m_translation = md->vTranslation;
    m_rotation = md->vRotation;
    m_scaling = md->vScaling;
}

int GLRenderableMesh::GetVertexBufferSize(int submeshid)
{
    return m_SubMeshTriangleCount[submeshid]*3*sizeof(float)*4; // each vertex is 4 floats (X, Y, Z, W)
}

int GLRenderableMesh::GetNormalBufferSize(int submeshid)
{
    return m_SubMeshTriangleCount[submeshid]*3*sizeof(float)*3; // each vertex is 3 floats (X, Y, Z)
}

int GLRenderableMesh::GetColorBufferSize(int submeshid)
{
    return m_SubMeshTriangleCount[submeshid]*3*sizeof(float)*4;  // RGBA!
}

int GLRenderableMesh::GetTextureBufferSize(int submeshid)
{
    return m_SubMeshTriangleCount[submeshid]*3*sizeof(float)*2; // each UV is 2 floats
}

int GLRenderableMesh::GetIndexBufferSize(int submeshid)
{
    return m_SubMeshTriangleCount[submeshid]*3*sizeof(int);
}

int GLRenderableMesh::GetElementCount(int submeshid)
{
    return m_SubMeshTriangleCount[submeshid]*3;
}

void GLRenderableMesh::LoadVBO()
{
    nux::Vec4<float> *VertexData;
    nux::Vec3<float> *NormalData;
    nux::Vec3<float> *TangentData;
    nux::Vec3<float> *BinormalData;
    nux::Vec4<float> *ColorData;
    nux::Vec2<float> *TexCoord0;
    int *indexdata;
    int size;

    if(m_SubMeshCount == 0)
        return;
 
    // vertex data
    size = GetVertexBufferSize();
    VertexData = m_VertexArray[0];
    
    glGenBuffersARB(1, &m_GLVBOVertex);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOVertex);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, VertexData, GL_STATIC_DRAW_ARB);

    // texture data
    if(HasTexCoord())
    {
        size = GetTextureBufferSize();
        TexCoord0 = m_UVArray[0][0];
        // Generate And Bind The Texture Coordinate Buffer
        glGenBuffersARB( 1, &m_GLVBOTexture0 );
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_GLVBOTexture0 );
        glBufferDataARB( GL_ARRAY_BUFFER_ARB, size, TexCoord0, GL_STATIC_DRAW_ARB );
    }
    
    // Normal data
    if(HasNormals())
    {
        size = GetNormalBufferSize();
        NormalData = m_NormalArray[0];
        glGenBuffersARB(1, &m_GLVBONormal);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBONormal);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, NormalData, GL_STATIC_DRAW_ARB);
    }

    if(HasTangentSpace())
    {
        size = GetNormalBufferSize();
        TangentData = m_TangentArray[0];
        glGenBuffersARB(1, &m_GLVBOTangent);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOTangent);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, TangentData, GL_STATIC_DRAW_ARB);


        size = GetNormalBufferSize();
        BinormalData = m_BinormalArray[0];
        glGenBuffersARB(1, &m_GLVBOBiNormal);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOBiNormal);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, BinormalData, GL_STATIC_DRAW_ARB);
    }

    if(HasColors())
    {
        size = GetColorBufferSize();
        ColorData = m_ColorArray[0];
        glGenBuffersARB(1, &m_GLVBOColor);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOColor);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, ColorData, GL_STATIC_DRAW_ARB);
    }

    // index data
    size = GetIndexBufferSize();
    indexdata = m_IndexArray[0];
    glGenBuffersARB( 1, &m_GLVBOIndexVertex);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_GLVBOIndexVertex);  
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, size, indexdata, GL_STATIC_DRAW_ARB);
}

void GLRenderableMesh::DrawVBO()
{
    if(m_SubMeshCount == 0)
        return;

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOVertex);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(4, GL_FLOAT, 0, NULL);

    if(HasTexCoord()) 
    {
        CHECKGLX( glClientActiveTextureARB(GL_TEXTURE0) );
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_GLVBOTexture0 );      
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);
    }

    if(HasNormals()) 
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBONormal);
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, NULL);
    }

    if(HasColors()) 
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOColor);
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, NULL);
    }

    if(HasTangentSpace())
    {
//        glActiveTextureARB (GL_TEXTURE1_ARB);
//        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOTangent);
//        glTexCoordPointer(3, GL_FLOAT, 0, NULL);
//
//        glActiveTextureARB (GL_TEXTURE2_ARB);
//        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOBiNormal);
//        glTexCoordPointer(3, GL_FLOAT, 0, NULL);
    }


    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_GLVBOIndexVertex);

    glDrawElements( GL_TRIANGLES, GetElementCount(),
        GL_UNSIGNED_INT, NULL);		// Draw All Of The Triangles At Once

//    GLint aaa;
//    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &aaa);
//    
//    glDrawRangeElements(GL_TRIANGLES, 
//        m_ArrayStats[0].MinIndex,
//        m_ArrayStats[0].MaxIndex,
//        m_ArrayStats[0].nbTriangles * 3, // if rendering GL_TRIANGLES, then the number of index is nbTriangles * 3
//        GL_UNSIGNED_INT, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY);
    if(HasTexCoord()) 
    {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if(HasNormals()) 
    {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    if(HasColors()) 
    {
        glDisableClientState(GL_COLOR_ARRAY);
    }

//    glActiveTextureARB (GL_TEXTURE0);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//    glActiveTextureARB (GL_TEXTURE1_ARB);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//    glActiveTextureARB (GL_TEXTURE2_ARB);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//    glActiveTextureARB (GL_TEXTURE3_ARB);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);



}

// Set the vertex attribute name at the given index for the ginven program.
// Return the incremented(+1) index attribute for the next attribute name.
int GLRenderableMesh::setVertexAttribute(unsigned int shader_program, int attribute_index, const char* attribute_name)
{
    //glBindAttribLocationARB(shader_program, attribute_index, attribute_name);
    return attribute_index+1;
}

// void GLRenderableMesh::renderVertexAttributes(GLProgramObject* shaderProgram/*, unsigned int shader_prg*/)
// {
// 
//     // We are using vertex buffer object, but the vertex parameters are passed as attributes.
//     // So disable the client parameters state:
//     //      glDisableClientState(GL_COLOR_ARRAY);
//     //      glDisableClientState(GL_EDGE_FLAG_ARRAY);
//     //      glDisableClientState(GL_INDEX_ARRAY);
//     //      glDisableClientState(GL_NORMAL_ARRAY);
//     //      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//     //      glDisableClientState(GL_VERTEX_ARRAY);
//     //GLDisableClientState();
// 
// 
//     unsigned int shader_prg = shaderProgram->GetProgramObj();
//     int vertex_attribute_index = 0;
// 
// 
//     vertex_attribute_index = shaderProgram->GetAttributeLocation(POSITION);// glGetAttribLocationARB(shader_prg, "myposition");
//     glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOVertex);
//     glEnableVertexAttribArrayARB(vertex_attribute_index); // Position
//     glVertexAttribPointerARB(vertex_attribute_index, 4, GL_FLOAT, GL_FALSE, 0, NULL);
//     //vertex_attribute_index = setVertexAttribute(shader_prg, vertex_attribute_index, "myvertex");
// 
//     if(HasNormals()) 
//     {
//         vertex_attribute_index = shaderProgram->GetAttributeLocation(NORMAL);
//         glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBONormal);
//         glEnableVertexAttribArrayARB(vertex_attribute_index); // Normal
//         glVertexAttribPointerARB(vertex_attribute_index, 3, GL_FLOAT, GL_FALSE, 0, NULL);
// 
//         //vertex_attribute_index = setVertexAttribute(shader_prg, vertex_attribute_index, "mynormal");
//     }
//     
//     if(HasTangentSpace()) 
//     {
//         vertex_attribute_index = shaderProgram->GetAttributeLocation(TANGENT);
//         glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOTangent);
//         glEnableVertexAttribArrayARB(vertex_attribute_index); // Tangent
//         glVertexAttribPointerARB(vertex_attribute_index, 3, GL_FLOAT, false, 0, NULL);
//         //vertex_attribute_index = setVertexAttribute(shader_prg, vertex_attribute_index, "mytangent");
// 
//         vertex_attribute_index = shaderProgram->GetAttributeLocation(BINORMAL);
//         glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOBiNormal);
//         glEnableVertexAttribArrayARB(vertex_attribute_index); // Binormal
//         glVertexAttribPointerARB(vertex_attribute_index, 3, GL_FLOAT, false, 0, NULL);
//         //vertex_attribute_index = setVertexAttribute(shader_prg, vertex_attribute_index, "mybinormal");
//     }
// 
//     if(HasColors()) 
//     {
//         vertex_attribute_index = shaderProgram->GetAttributeLocation(COLOR);
//         glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOColor);
//         glEnableVertexAttribArrayARB(vertex_attribute_index); // texcoord
//         glVertexAttribPointerARB(vertex_attribute_index, 4, GL_FLOAT, GL_FALSE, 0, NULL);
//         //vertex_attribute_index = setVertexAttribute(shader_prg, vertex_attribute_index, "mycolor");
//     }
// 
//     if(HasTexCoord())
//     {
//         vertex_attribute_index = shaderProgram->GetAttributeLocation(TEXTURE);
//         glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVBOTexture0);
//         glEnableVertexAttribArrayARB(vertex_attribute_index); // texcoord
//         glVertexAttribPointerARB(vertex_attribute_index, 2, GL_FLOAT, GL_FALSE, 0, NULL);
//         //vertex_attribute_index = setVertexAttribute(shader_prg, vertex_attribute_index, "mytexture");
//     }
// 
//     glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_GLVBOIndexVertex);
//     glDrawElements( GL_TRIANGLES, GetElementCount(), GL_UNSIGNED_INT, NULL);		// Draw All Of The Triangles At Once
// 
//     for(int att = 0; att < vertex_attribute_index; att++)
//         glDisableVertexAttribArrayARB(att);
// }


nux::Matrix4 GLRenderableMesh::getLocalToWorldMatrix()
{
    nux::Matrix4 m, s, t, rx, ry, rz;
    m.Identity();

    s.Scale(m_scaling.x, m_scaling.y, m_scaling.z);
    t.Translate(m_translation.x, m_translation.y, m_translation.z);
    rx.Rotate_x(m_rotation.x * nux::Const::pi / 180.0f);
    ry.Rotate_y(m_rotation.y * nux::Const::pi / 180.0f);
    rz.Rotate_z(m_rotation.z * nux::Const::pi / 180.0f);

    //m = t * rx * ry * rz * s;

    return m;
}

void GLRenderableMesh::CalculateTangentArray(const nux::Vec4<float> *vertex,
                           const nux::Vec3<float> *normal,
                           const nux::Vec2<float> *texcoord,
                           long triangleCount,
                           const int *triangleIndex,
                           nux::Vec3<float> *tangent,
                           nux::Vec3<float> *binormal)
{
    int vertexCount = triangleCount*3;
    nux::Vec3<float> *tan1 = new nux::Vec3<float>[vertexCount * 2];
    nux::Vec3<float> *tan2 = tan1 + vertexCount;
    memset(tan1, 0, vertexCount * sizeof(nux::Vec3<float>) * 2);

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
        nux::Vec3<float> sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
        nux::Vec3<float> tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

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

        nux::Vector3 N(n.x, n.y, n.z);
        nux::Vector3 T1(t1.x, t1.y, t1.z);
        nux::Vector3 T2(t2.x, t2.y, t2.z);

        // Gram-Schmidt orthogonalize
        nux::Vector3 Tangent = T1 - (N.DotProduct(T1))*N;
        Tangent.Normalize();
        tangent[a] = nux::Vec3<float>(Tangent.x, Tangent.y, Tangent.z);

        // Calculate handedness
        float handness = (((N.CrossProduct(T1)).DotProduct(T2)) < 0.0F) ? -1.0F : 1.0F;

        nux::Vector3 Binormal = handness* N.CrossProduct(Tangent);
        binormal[a] = nux::Vec3<float>(Binormal.x, Binormal.y, Binormal.z);

        binormal[a].Normalize();
    }
    delete[] tan1;
}

void GLRenderableMesh::RenderNormal()
{
    // Push the mesh local to world matrix.
    glPushMatrix();
    glTranslatef(m_translation.x, m_translation.y, m_translation.z);
    glScalef(m_scaling.x, m_scaling.y, m_scaling.z);
    glRotatef(m_rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(m_rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(m_rotation.z, 0.0f, 0.0f, 1.0f);

    for(unsigned int Primitive = 0; Primitive < m_SubMeshCount; Primitive++)
    {
        int nbTriangle = m_SubMeshTriangleCount[Primitive];

        for(long j = 0; j < nbTriangle; j++)
        {
            glColor4f(1,0,0,1);
            /*glBegin(GL_TRIANGLES);
            glVertex3f(m_VertexArray[Primitive][3*j].x, m_VertexArray[Primitive][3*j].y, m_VertexArray[Primitive][3*j].z);
            glVertex3f(m_VertexArray[Primitive][3*j+1].x, m_VertexArray[Primitive][3*j+1].y, m_VertexArray[Primitive][3*j+1].z);
            glVertex3f(m_VertexArray[Primitive][3*j+2].x, m_VertexArray[Primitive][3*j+2].y, m_VertexArray[Primitive][3*j+2].z);
            glEnd();*/

            glBegin(GL_LINES);
            glVertex3f(m_VertexArray[Primitive][3*j].x, m_VertexArray[Primitive][3*j].y, m_VertexArray[Primitive][3*j].z);
            glVertex3f(m_VertexArray[Primitive][3*j].x + m_NormalArray[Primitive][3*j].x,
                m_VertexArray[Primitive][3*j].y + m_NormalArray[Primitive][3*j].y,
                m_VertexArray[Primitive][3*j].z + m_NormalArray[Primitive][3*j].z);

            glVertex3f(m_VertexArray[Primitive][3*j+1].x, m_VertexArray[Primitive][3*j+1].y, m_VertexArray[Primitive][3*j+1].z);
            glVertex3f(m_VertexArray[Primitive][3*j+1].x + m_NormalArray[Primitive][3*j+1].x,
                m_VertexArray[Primitive][3*j+1].y + m_NormalArray[Primitive][3*j+1].y,
                m_VertexArray[Primitive][3*j+1].z + m_NormalArray[Primitive][3*j+1].z);

            glVertex3f(m_VertexArray[Primitive][3*j+2].x, m_VertexArray[Primitive][3*j+2].y, m_VertexArray[Primitive][3*j+2].z);
            glVertex3f(m_VertexArray[Primitive][3*j+2].x + m_NormalArray[Primitive][3*j+2].x,
                m_VertexArray[Primitive][3*j+2].y + m_NormalArray[Primitive][3*j+2].y,
                m_VertexArray[Primitive][3*j+2].z + m_NormalArray[Primitive][3*j+2].z);

            glEnd();


            glColor4f(0,1,0,1);
            /*glBegin(GL_TRIANGLES);
            glVertex3f(m_VertexArray[Primitive][3*j].x, m_VertexArray[Primitive][3*j].y, m_VertexArray[Primitive][3*j].z);
            glVertex3f(m_VertexArray[Primitive][3*j+1].x, m_VertexArray[Primitive][3*j+1].y, m_VertexArray[Primitive][3*j+1].z);
            glVertex3f(m_VertexArray[Primitive][3*j+2].x, m_VertexArray[Primitive][3*j+2].y, m_VertexArray[Primitive][3*j+2].z);
            glEnd();*/

            glBegin(GL_LINES);
            glVertex3f(m_VertexArray[Primitive][3*j].x, m_VertexArray[Primitive][3*j].y, m_VertexArray[Primitive][3*j].z);
            glVertex3f(m_VertexArray[Primitive][3*j].x + m_BinormalArray[Primitive][3*j].x,
                m_VertexArray[Primitive][3*j].y + m_BinormalArray[Primitive][3*j].y,
                m_VertexArray[Primitive][3*j].z + m_BinormalArray[Primitive][3*j].z);

            glVertex3f(m_VertexArray[Primitive][3*j+1].x, m_VertexArray[Primitive][3*j+1].y, m_VertexArray[Primitive][3*j+1].z);
            glVertex3f(m_VertexArray[Primitive][3*j+1].x + m_BinormalArray[Primitive][3*j+1].x,
                m_VertexArray[Primitive][3*j+1].y + m_BinormalArray[Primitive][3*j+1].y,
                m_VertexArray[Primitive][3*j+1].z + m_BinormalArray[Primitive][3*j+1].z);

            glVertex3f(m_VertexArray[Primitive][3*j+2].x, m_VertexArray[Primitive][3*j+2].y, m_VertexArray[Primitive][3*j+2].z);
            glVertex3f(m_VertexArray[Primitive][3*j+2].x + m_BinormalArray[Primitive][3*j+2].x,
                m_VertexArray[Primitive][3*j+2].y + m_BinormalArray[Primitive][3*j+2].y,
                m_VertexArray[Primitive][3*j+2].z + m_BinormalArray[Primitive][3*j+2].z);

            glEnd();


            glColor4f(0,0,1,1);
            glBegin(GL_LINES);
            glVertex3f(m_VertexArray[Primitive][3*j].x, m_VertexArray[Primitive][3*j].y, m_VertexArray[Primitive][3*j].z);
            glVertex3f(m_VertexArray[Primitive][3*j].x + m_TangentArray[Primitive][3*j].x,
                m_VertexArray[Primitive][3*j].y + m_TangentArray[Primitive][3*j].y,
                m_VertexArray[Primitive][3*j].z + m_TangentArray[Primitive][3*j].z);

            glVertex3f(m_VertexArray[Primitive][3*j+1].x, m_VertexArray[Primitive][3*j+1].y, m_VertexArray[Primitive][3*j+1].z);
            glVertex3f(m_VertexArray[Primitive][3*j+1].x + m_TangentArray[Primitive][3*j+1].x,
                m_VertexArray[Primitive][3*j+1].y + m_TangentArray[Primitive][3*j+1].y,
                m_VertexArray[Primitive][3*j+1].z + m_TangentArray[Primitive][3*j+1].z);

            glVertex3f(m_VertexArray[Primitive][3*j+2].x, m_VertexArray[Primitive][3*j+2].y, m_VertexArray[Primitive][3*j+2].z);
            glVertex3f(m_VertexArray[Primitive][3*j+2].x + m_TangentArray[Primitive][3*j+2].x,
                m_VertexArray[Primitive][3*j+2].y + m_TangentArray[Primitive][3*j+2].y,
                m_VertexArray[Primitive][3*j+2].z + m_TangentArray[Primitive][3*j+2].z);

            glEnd();
        }
//        m_SubMeshTriangleCount[Primitive] = nbTriangle;
//        m_IndexArray[Primitive] = new int[nbTriangle*3];
//
//        // After that the VertexArray for primitive #Primitive is like:
//        // [Triangle0<v0,v1,v2>, Triangle1<v0,v1,v2> .... TriangleN<v0,v1,v2>]
//        // The index array is:
//        // [Triangle0<0,1,2>, Triangle1<3,4,5>, ..... TriangleN<3N,3N+1,3N+2>] 
//        long nbIndex = nbTriangle * 3;
//        for(long index = 0; index < nbIndex; index++)
//        {
//            m_IndexArray[Primitive][index] = index;
//        }
    }

    // Pop the mesh local to world matrix.
    glPopMatrix();
}


