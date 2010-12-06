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


#ifndef NMESHARCHIVER_VER_0_0_1_H
#define NMESHARCHIVER_VER_0_0_1_H

namespace nux
{
  class NMeshObject;

  typedef enum
  {
    MESHINDEX_UNKNOWN = 0,
    MESHINDEX_SHORT,
    MESHINDEX_INT,
    FORCE_MESHINDEXTYPE = 0x7fffffff
  }  MeshIndexType;

  typedef enum
  {
    MESHBUFFER_UNKNOWN = 0,
    MESHBUFFER_INDEXBUFFER,
    MESHBUFFER_VERTEXBUFFER,
    FORCE_MESHBUFFER = 0x7fffffff
  }  MeshBufferType;

  typedef enum
  {
    POLYGONTYPE_UNKNOWN = 0,
    POLYGONTYPE_TRIANGLE,
    POLYGONTYPE_TRIANGLESTRIP,
    POLYGONTYPE_TRIANGLEFAN,
    POLYGONTYPE_QUAD,
    POLYGONTYPE_QUADSTRIP,
    POLYGONTYPE_QUADFAN,
    POLYGONTYPE_LINE,
    POLYGONTYPE_LINESTRIP,
    POLYGONTYPE_POINT,
    FORCE_POLYGONTYPE = 0x7fffffff
  }  MeshPolygonType;

  typedef enum
  {
    VERTEXATTRIB_UNKNOWN = 0,
    VERTEXATTRIB_FLOAT,
    VERTEXATTRIB_INT,
    VERTEXATTRIB_BYTE,
    VERTEXATTRIB_UBYTE,
    VERTEXATTRIB_BYTE_N,    // the byte component should be divided by 128.0 to get the normalized value in the range [-1.0, 1.0].
    VERTEXATTRIB_UBYTE_N,   // the byte component should be divided by 255.0 to get the normalized value in the range [-1.0, 1.0].
    VERTEXATTRIB_SHORT,
    VERTEXATTRIB_USHORT,
    VERTEXATTRIB_SHORT_N,    // the short component should be divided by 32767.0 to get the normalized value in the range [-1.0, 1.0].
    VERTEXATTRIB_USHORT_N,   // the short component should be divided by 65535.0 to get the normalized value in the range [0.0, 1.0].
    FORCE_VERTEXATTRIB = 0x7fffffff
  }  VertexAttribType;

  typedef enum
  {
    VBSEMANTIC_UNKNOWN = 0,
    VBSEMANTIC_POSITION,
    VBSEMANTIC_NORMAL,
    VBSEMANTIC_COLOR0,
    VBSEMANTIC_COLOR1,
    VBSEMANTIC_TANGENT,
    VBSEMANTIC_BINORMAL,
    VBSEMANTIC_TEXCOORD0,
    VBSEMANTIC_TEXCOORD1,
    VBSEMANTIC_TEXCOORD2,
    VBSEMANTIC_TEXCOORD3,
    VBSEMANTIC_TEXCOORD4,
    VBSEMANTIC_TEXCOORD5,
    VBSEMANTIC_TEXCOORD6,
    VBSEMANTIC_TEXCOORD7,
    FORCE_VBSEMANTIC = 0x7fffffff
  }  VertexBufferSemantic;

  typedef struct
  {
    nux::t_s64 FilePointer;
    NFileName Name;
    unsigned int Type;
    unsigned int Format;
    unsigned int Width;
    unsigned int Height;
    unsigned int Depth;
    unsigned int NumMipmap;
    unsigned int Size;

  } MeshDataInfo_ver_0_0_1;

  bool SaveMeshObject_ver_0_0_1 (nux::NSerializer *FileStream, NMeshObject *);
  NMeshObject *LoadMeshObject_ver_0_0_1 (nux::NSerializer *FileStream, unsigned int Offset);
  void MeshDataGetInfo_ver_0_0_1 (nux::NSerializer *FileStream, MeshDataInfo_ver_0_0_1 &TextureArchiveInfo);

}

#endif // NMESHARCHIVER_VER_0_0_1_H
