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
#include "NuxCore/FilePath.h"

#include "NMeshArchiver_ver_0_0_1.h"
#include "NWorldArchiveManager.h"
#include "NMeshObject.h"

namespace nux
{

  bool SaveMeshObject_ver_0_0_1 (nux::NSerializer *FileStream, NMeshObject *mesh)
  {
    /*
        ObjectType: OBJECTTYPE_MESH
        MeshPolygonType: MESHPRIMITIVE_TRIANGLE
        NumPrimitive: 45
        NumIndex: 45*3
        MeshBufferType                  MeshIndexType   VertexAttribType       VertexBufferSemantic  NumComponent
        ------------------------------------------------------------------------------------------------------------------------------
        MESHBUFFER_INDEXBUFFER          MESHINDEX_INT                                                               BufferSize [....]
        MESHBUFFER_VERTEXBUFFER                         VERTEXATTRIB_FLOAT     VBSEMANTIC_POSITION        4         BufferSize [....]
        MESHBUFFER_VERTEXBUFFER                         VERTEXATTRIB_FLOAT     VBSEMANTIC_NORMAL          3         BufferSize [....]
        MESHBUFFER_VERTEXBUFFER                         VERTEXATTRIB_FLOAT     VBSEMANTIC_COLOR           4         BufferSize [....]
        MESHBUFFER_VERTEXBUFFER                         VERTEXATTRIB_FLOAT     VBSEMANTIC_TANGENT         3         BufferSize [....]
        MESHBUFFER_VERTEXBUFFER                         VERTEXATTRIB_FLOAT     VBSEMANTIC_BINORMAL        3         BufferSize [....]
        MESHBUFFER_VERTEXBUFFER                         VERTEXATTRIB_FLOAT     VBSEMANTIC_TEXCOORD0       4         BufferSize [....]
        MESHBUFFER_VERTEXBUFFER                         VERTEXATTRIB_FLOAT     VBSEMANTIC_TEXCOORD1       4         BufferSize [....]
    */

    //------------------------------------------------------------------------------------------------------------------------
//     ObjectType objecttype = OBJECTTYPE_MESH;
//     FileStream->Serialize((char*) &objecttype,      sizeof(objecttype));

    MeshPolygonType primtype = POLYGONTYPE_TRIANGLE;
    FileStream->Serialize ( (char *) &primtype,        sizeof (primtype) );

    int numprimitive = mesh->GetNumPolygon();
    FileStream->Serialize ( (char *) &numprimitive,    sizeof (numprimitive) );

    //------------------------------------------------------------------------------------------------------------------------
    // Save World Matrix here
    FileStream->Serialize ( (char *) &mesh->m_WorldMatrix,    16 * 4);
    //------------------------------------------------------------------------------------------------------------------------
    MeshBufferType buffertype   = MESHBUFFER_INDEXBUFFER;
    MeshIndexType indextype     = MESHINDEX_INT;
    int Size                    = mesh->GetIndexBufferSize();

    FileStream->Serialize ( (char *) &buffertype,                  sizeof (buffertype) );
    FileStream->Serialize ( (char *) &indextype,                   sizeof (indextype) );
    FileStream->Serialize ( (char *) &mesh->m_MinIndex,            sizeof (mesh->m_MinIndex) );
    FileStream->Serialize ( (char *) &mesh->m_MaxIndex,            sizeof (mesh->m_MaxIndex) );
    FileStream->Serialize ( (char *) &Size,                        sizeof (Size) );
    FileStream->Serialize ( (char *) &mesh->m_IndexArray[0],       Size);

    //------------------------------------------------------------------------------------------------------------------------
    buffertype                          = MESHBUFFER_VERTEXBUFFER;
    VertexAttribType attribtype         = VERTEXATTRIB_FLOAT;
    VertexBufferSemantic buffersemantic = VBSEMANTIC_POSITION;
    unsigned int numComponent           = 4;
    unsigned int numBuffer              = 1;
    Size                                = mesh->GetVertexBufferSize();

    FileStream->Serialize ( (char *) &buffersemantic,                  sizeof (buffersemantic) );
    FileStream->Serialize ( (char *) &buffertype,                      sizeof (buffertype) );
    FileStream->Serialize ( (char *) &numBuffer,                       sizeof (numBuffer) );
    FileStream->Serialize ( (char *) &attribtype,                      sizeof (attribtype) );
    FileStream->Serialize ( (char *) &numComponent,                    sizeof (numComponent) );
    FileStream->Serialize ( (char *) &Size,                            sizeof (Size) );
    FileStream->Serialize ( (char *) mesh->m_VertexArray,              Size);

    if (mesh->HasNormal() )
    {
      buffertype      = MESHBUFFER_VERTEXBUFFER;
      buffersemantic  = VBSEMANTIC_NORMAL;
      numBuffer       = mesh->NumNormalAttributes;

      FileStream->Serialize ( (char *) &buffersemantic,                  sizeof (buffersemantic) );
      FileStream->Serialize ( (char *) &buffertype,                      sizeof (buffertype) );
      FileStream->Serialize ( (char *) &numBuffer,                   sizeof (numBuffer) );

      for (unsigned int i = 0; i < numBuffer; i++)
      {
        attribtype      = VERTEXATTRIB_FLOAT;
        numComponent    = 3;
        Size            = mesh->GetNormalBufferSize();

        FileStream->Serialize ( (char *) &attribtype,                  sizeof (attribtype) );
        FileStream->Serialize ( (char *) &numComponent,                sizeof (numComponent) );
        FileStream->Serialize ( (char *) &Size,                        sizeof (Size) );
        FileStream->Serialize ( (char *) mesh->m_NormalArray[i],       Size);
      }
    }

    if (mesh->HasTangent() )
    {
      buffertype      = MESHBUFFER_VERTEXBUFFER;
      buffersemantic  = VBSEMANTIC_TANGENT;
      numBuffer       = mesh->NumTangentAttributes;

      FileStream->Serialize ( (char *) &buffersemantic,                  sizeof (buffersemantic) );
      FileStream->Serialize ( (char *) &buffertype,                      sizeof (buffertype) );
      FileStream->Serialize ( (char *) &numBuffer,                       sizeof (numBuffer) );

      for (unsigned int i = 0; i < numBuffer; i++)
      {
        attribtype      = VERTEXATTRIB_FLOAT;
        numComponent    = 3;
        Size            = mesh->GetNormalBufferSize();

        FileStream->Serialize ( (char *) &attribtype,                  sizeof (attribtype) );
        FileStream->Serialize ( (char *) &numComponent,                sizeof (numComponent) );
        FileStream->Serialize ( (char *) &Size,                        sizeof (Size) );
        FileStream->Serialize ( (char *) mesh->m_TangentArrayXSI[i],   Size);
      }
    }

    if (mesh->HasBinormal() )
    {
      buffertype      = MESHBUFFER_VERTEXBUFFER;
      buffersemantic  = VBSEMANTIC_BINORMAL;
      numBuffer       = mesh->NumBinormalAttributes;

      FileStream->Serialize ( (char *) &buffersemantic,                  sizeof (buffersemantic) );
      FileStream->Serialize ( (char *) &buffertype,                      sizeof (buffertype) );
      FileStream->Serialize ( (char *) &numBuffer,                       sizeof (numBuffer) );

      for (unsigned int i = 0; i < numBuffer; i++)
      {
        attribtype      = VERTEXATTRIB_FLOAT;
        numComponent    = 3;
        Size            = mesh->GetNormalBufferSize();

        FileStream->Serialize ( (char *) &attribtype,                  sizeof (attribtype) );
        FileStream->Serialize ( (char *) &numComponent,                sizeof (numComponent) );
        FileStream->Serialize ( (char *) &Size,                        sizeof (Size) );
        FileStream->Serialize ( (char *) mesh->m_BinormalArrayXSI[i],  Size);
      }
    }

    if (mesh->HasTexCoord() )
    {
      buffertype      = MESHBUFFER_VERTEXBUFFER;
      buffersemantic  = VBSEMANTIC_TEXCOORD0;
      numBuffer       = mesh->NumTextureAttributes;

      FileStream->Serialize ( (char *) &buffersemantic,                  sizeof (buffersemantic) );
      FileStream->Serialize ( (char *) &buffertype,                      sizeof (buffertype) );
      FileStream->Serialize ( (char *) &numBuffer,                       sizeof (numBuffer) );

      for (unsigned int i = 0; i < numBuffer; i++)
      {
        attribtype      = VERTEXATTRIB_FLOAT;
        numComponent    = 2;
        Size            = mesh->GetTextureBufferSize();

        FileStream->Serialize ( (char *) &attribtype,                  sizeof (attribtype) );
        FileStream->Serialize ( (char *) &numComponent,                sizeof (numComponent) );
        FileStream->Serialize ( (char *) &Size,                        sizeof (Size) );
        FileStream->Serialize ( (char *) mesh->m_TextureArray[i],      Size);
      }
    }

    if (mesh->HasColor() )
    {
      buffertype      = MESHBUFFER_VERTEXBUFFER;
      buffersemantic  = VBSEMANTIC_COLOR0;
      numBuffer       = mesh->NumColorAttributes;

      FileStream->Serialize ( (char *) &buffersemantic,                  sizeof (buffersemantic) );
      FileStream->Serialize ( (char *) &buffertype,                      sizeof (buffertype) );
      FileStream->Serialize ( (char *) &numBuffer,                       sizeof (numBuffer) );

      for (unsigned int i = 0; i < numBuffer; i++)
      {
        attribtype      = VERTEXATTRIB_FLOAT;
        numComponent    = 4;
        Size            = mesh->GetColorBufferSize();
        FileStream->Serialize ( (char *) &attribtype,                  sizeof (attribtype) );
        FileStream->Serialize ( (char *) &numComponent,                sizeof (numComponent) );
        FileStream->Serialize ( (char *) &Size,                        sizeof (Size) );
        FileStream->Serialize ( (char *) mesh->m_ColorArray[i],        Size);
      }
    }

    // Mark the End of the Mesh with a VBSEMANTIC_UNKNOWN
    buffersemantic = VBSEMANTIC_UNKNOWN;
    FileStream->Serialize ( (char *) &buffersemantic,        sizeof (buffersemantic) );

    return true;
  }

  NMeshObject *LoadMeshObject_ver_0_0_1 (nux::NSerializer *FileStream, unsigned int Offset)
  {
    NMeshObject *mesh = new NMeshObject (0);

    //------------------------------------------------------------------------------------------------------------------------
//     ObjectType objecttype = OBJECTTYPE_UNKNOWN;
//     FileStream->Serialize((char*) &objecttype,      sizeof(objecttype));

    MeshPolygonType primtype = POLYGONTYPE_UNKNOWN;
    FileStream->Serialize ( (char *) &primtype,                sizeof (primtype) );

    FileStream->Serialize ( (char *) &mesh->m_NumPolygon,      sizeof (mesh->m_NumPolygon) );

    //------------------------------------------------------------------------------------------------------------------------
    // Load World Matrix here
    FileStream->Serialize ( (char *) &mesh->m_WorldMatrix,    16 * 4);
    //------------------------------------------------------------------------------------------------------------------------
    MeshBufferType buffertype   = MESHBUFFER_UNKNOWN;
    MeshIndexType indextype     = MESHINDEX_UNKNOWN;
    int Size                    = 0;

    FileStream->Serialize ( (char *) &buffertype,                          sizeof (buffertype) );
    FileStream->Serialize ( (char *) &indextype,                           sizeof (indextype) );
    FileStream->Serialize ( (char *) &mesh->m_MinIndex,                    sizeof (mesh->m_MinIndex) );
    FileStream->Serialize ( (char *) &mesh->m_MaxIndex,                    sizeof (mesh->m_MaxIndex) );
    FileStream->Serialize ( (char *) &Size,                                sizeof (Size) );

    mesh->m_IndexArray.resize (Size);
    FileStream->Serialize ( (char *) & (mesh->m_IndexArray[0]), Size);

    //------------------------------------------------------------------------------------------------------------------------
    buffertype                          = MESHBUFFER_UNKNOWN;
    VertexAttribType attribtype         = VERTEXATTRIB_UNKNOWN;
    VertexBufferSemantic buffersemantic = VBSEMANTIC_UNKNOWN;
    unsigned int numComponent           = 0;
    unsigned int numBuffer              = 0;
    Size                                = mesh->GetVertexBufferSize();

    FileStream->Serialize ( (char *) &buffersemantic,                      sizeof (buffersemantic) );
    FileStream->Serialize ( (char *) &buffertype,                          sizeof (buffertype) );
    FileStream->Serialize ( (char *) &numBuffer,                           sizeof (numBuffer) );
    FileStream->Serialize ( (char *) &attribtype,                          sizeof (attribtype) );
    FileStream->Serialize ( (char *) &numComponent,                        sizeof (numComponent) );
    FileStream->Serialize ( (char *) &Size,                                sizeof (Size) );

    mesh->m_VertexArray = (nux::Vec4<float>*) new char[Size];
    FileStream->Serialize ( (char *) mesh->m_VertexArray,                  Size);


    buffersemantic = VBSEMANTIC_UNKNOWN;
    FileStream->Serialize ( (char *) &buffersemantic,                      sizeof (buffersemantic) );

    while (buffersemantic != VBSEMANTIC_UNKNOWN)
    {
      if (buffersemantic == VBSEMANTIC_NORMAL)
      {
        buffertype      = MESHBUFFER_UNKNOWN;
        //buffersemantic  = VBSEMANTIC_UNKNOWN;
        numBuffer       = 0;

        //FileStream->Serialize((char*) &buffersemantic,                  sizeof(buffersemantic));
        FileStream->Serialize ( (char *) &buffertype,                      sizeof (buffertype) );
        FileStream->Serialize ( (char *) &numBuffer,                       sizeof (numBuffer) );

        mesh->NumNormalAttributes = numBuffer;
        mesh->m_NormalArray = new nux::Vec3<float>*[numBuffer];

        for (unsigned int i = 0; i < numBuffer; i++)
          mesh->m_NormalArray[i] = 0;

        for (unsigned int i = 0; i < numBuffer; i++)
        {
          attribtype      = VERTEXATTRIB_UNKNOWN;
          numComponent    = 0;
          Size            = 0;

          FileStream->Serialize ( (char *) &attribtype,                  sizeof (attribtype) );
          FileStream->Serialize ( (char *) &numComponent,                sizeof (numComponent) );
          FileStream->Serialize ( (char *) &Size,                        sizeof (Size) );

          mesh->m_NormalArray[i] = (nux::Vec3<float>*) new char[Size];
          FileStream->Serialize ( (char *) mesh->m_NormalArray[i],       Size);
        }
      }

      if (buffersemantic == VBSEMANTIC_TANGENT)
      {
        buffertype      = MESHBUFFER_UNKNOWN;
        //buffersemantic  = VBSEMANTIC_UNKNOWN;
        numBuffer       = mesh->NumTangentAttributes;

        //FileStream->Serialize((char*) &buffersemantic,                  sizeof(buffersemantic));
        FileStream->Serialize ( (char *) &buffertype,                      sizeof (buffertype) );
        FileStream->Serialize ( (char *) &numBuffer,                       sizeof (numBuffer) );

        mesh->NumTangentAttributes = numBuffer;
        mesh->m_TangentArrayXSI = new nux::Vec3<float>*[numBuffer];

        for (unsigned int i = 0; i < numBuffer; i++)
          mesh->m_TangentArrayXSI[i] = 0;

        for (unsigned int i = 0; i < numBuffer; i++)
        {
          attribtype      = VERTEXATTRIB_UNKNOWN;
          numComponent    = 0;
          Size            = 0;

          FileStream->Serialize ( (char *) &attribtype,                  sizeof (attribtype) );
          FileStream->Serialize ( (char *) &numComponent,                sizeof (numComponent) );
          FileStream->Serialize ( (char *) &Size,                        sizeof (Size) );

          mesh->m_TangentArrayXSI[i] = (nux::Vec3<float>*) new char[Size];
          FileStream->Serialize ( (char *) mesh->m_TangentArrayXSI[i],   Size);
        }
      }

      if (buffersemantic == VBSEMANTIC_BINORMAL)
      {
        buffertype      = MESHBUFFER_UNKNOWN;
        //buffersemantic  = VBSEMANTIC_UNKNOWN;
        numBuffer       = mesh->NumBinormalAttributes;

        //FileStream->Serialize((char*) &buffersemantic,                  sizeof(buffersemantic));
        FileStream->Serialize ( (char *) &buffertype,                      sizeof (buffertype) );
        FileStream->Serialize ( (char *) &numBuffer,                       sizeof (numBuffer) );

        mesh->NumBinormalAttributes = numBuffer;
        mesh->m_BinormalArrayXSI = new nux::Vec3<float>*[numBuffer];

        for (unsigned int i = 0; i < numBuffer; i++)
          mesh->m_BinormalArrayXSI[i] = 0;

        for (unsigned int i = 0; i < numBuffer; i++)
        {
          attribtype      = VERTEXATTRIB_UNKNOWN;
          numComponent    = 0;
          Size            = 0;

          FileStream->Serialize ( (char *) &attribtype,                  sizeof (attribtype) );
          FileStream->Serialize ( (char *) &numComponent,                sizeof (numComponent) );
          FileStream->Serialize ( (char *) &Size,                        sizeof (Size) );

          mesh->m_BinormalArrayXSI[i] = (nux::Vec3<float>*) new char[Size];
          FileStream->Serialize ( (char *) mesh->m_BinormalArrayXSI[i],  Size);
        }
      }

      if (buffersemantic == VBSEMANTIC_TEXCOORD0)
      {
        buffertype      = MESHBUFFER_UNKNOWN;
        //buffersemantic  = VBSEMANTIC_UNKNOWN;
        numBuffer       = mesh->NumTextureAttributes;

        //FileStream->Serialize((char*) &buffersemantic,                  sizeof(buffersemantic));
        FileStream->Serialize ( (char *) &buffertype,                      sizeof (buffertype) );
        FileStream->Serialize ( (char *) &numBuffer,                       sizeof (numBuffer) );

        mesh->NumTextureAttributes = numBuffer;
        mesh->m_TextureArray = new nux::Vec2<float>*[numBuffer];

        for (unsigned int i = 0; i < numBuffer; i++)
          mesh->m_TextureArray[i] = 0;

        for (unsigned int i = 0; i < numBuffer; i++)
        {
          attribtype      = VERTEXATTRIB_UNKNOWN;
          numComponent    = 0;
          Size            = 0;

          FileStream->Serialize ( (char *) &attribtype,                  sizeof (attribtype) );
          FileStream->Serialize ( (char *) &numComponent,                sizeof (numComponent) );
          FileStream->Serialize ( (char *) &Size,                        sizeof (Size) );

          mesh->m_TextureArray[i] = (nux::Vec2<float>*) new char[Size];
          FileStream->Serialize ( (char *) mesh->m_TextureArray[i],      Size);
        }
      }

      if (buffersemantic == VBSEMANTIC_COLOR0)
      {
        buffertype      = MESHBUFFER_UNKNOWN;
        //buffersemantic  = VBSEMANTIC_UNKNOWN;
        numBuffer       = mesh->NumColorAttributes;

        //FileStream->Serialize((char*) &buffersemantic,                  sizeof(buffersemantic));
        FileStream->Serialize ( (char *) &buffertype,                      sizeof (buffertype) );
        FileStream->Serialize ( (char *) &numBuffer,                       sizeof (numBuffer) );

        mesh->NumColorAttributes = numBuffer;
        mesh->m_ColorArray = new nux::Vec4<float>*[numBuffer];

        for (unsigned int i = 0; i < numBuffer; i++)
          mesh->m_ColorArray[i] = 0;

        for (unsigned int i = 0; i < numBuffer; i++)
        {
          attribtype      = VERTEXATTRIB_UNKNOWN;
          numComponent    = 0;
          Size            = 0;
          FileStream->Serialize ( (char *) &attribtype,                  sizeof (attribtype) );
          FileStream->Serialize ( (char *) &numComponent,                sizeof (numComponent) );
          FileStream->Serialize ( (char *) &Size,                        sizeof (Size) );

          mesh->m_ColorArray[i] = (nux::Vec4<float>*) new char[Size];
          FileStream->Serialize ( (char *) mesh->m_ColorArray[i],        Size);
        }
      }

      buffersemantic = VBSEMANTIC_UNKNOWN;
      FileStream->Serialize ( (char *) &buffersemantic,                      sizeof (buffersemantic) );
    }

    return mesh;
  }

}

