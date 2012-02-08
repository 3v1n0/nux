
#include "GLResource.h"
#include "MeshFileLoader-OBJ.h"
#include "MeshData.h"
#include "NuxCore/Math/Vector3.h"
#include "NuxCore/Math/Vector2.h"

namespace nux
{
#define TOKEN_VERTEX_POS "v"
#define TOKEN_VERTEX_NOR "vn"
#define TOKEN_VERTEX_TEX "vt"
#define TOKEN_FACE "f"

  struct ObjMeshVertex
  {
    nux::Vector3 pos;
    nux::Vector2 texcoord;
    nux::Vector3 normal;
  };

  /* This is a triangle, that we can render */
  struct ObjMeshFace
  {
    ObjMeshVertex vertices[3];
  };

  /* This contains a list of triangles */
  struct ObjMesh
  {
    std::vector<ObjMeshFace> faces;
  };

  /* Internal structure */
  struct _ObjMeshFaceIndex
  {
    int pos_index[3];
    int tex_index[3];
    int nor_index[3];
  };

  MeshData* LoadMeshFile_OBJ(const char* filename)
  {
    std::vector<nux::Vector4>           positions;
    std::vector<nux::Vector2>           texcoords;
    std::vector<nux::Vector3>           normals;
    std::vector<_ObjMeshFaceIndex>  faces;
    /**
     * Load file, parse it
     * Lines beginning with:
     * '#'  are comments can be ignored
     * 'v'  are vertices positions(3 floats that can be positive or negative)
     * 'vt' are vertices texcoords(2 floats that can be positive or negative)
     * 'vn' are vertices normals   (3 floats that can be positive or negative)
     * 'f'  are faces, 3 values that contain 3 values which are separated by / and <space>
     */

    std::ifstream filestream;
    filestream.open(filename);

    std::string line_stream;    // No longer depending on char arrays thanks to: Dale Weiler

    while (std::getline(filestream, line_stream))
    {
      std::stringstream str_stream(line_stream);
      std::string type_str;
      str_stream >> type_str;

      if (type_str == TOKEN_VERTEX_POS)
      {
        nux::Vector4 pos;
        str_stream >> pos.x >> pos.y >> pos.z;
        pos.w = 1.0f;
        positions.push_back(pos);
      }
      else if (type_str == TOKEN_VERTEX_TEX)
      {
        nux::Vector2 tex;
        str_stream >> tex.x >> tex.y;
        texcoords.push_back(tex);
      }
      else if (type_str == TOKEN_VERTEX_NOR)
      {
        nux::Vector3 nor;
        str_stream >> nor.x >> nor.y >> nor.z;
        normals.push_back(nor);
      }
      else if (type_str == TOKEN_FACE)
      {
        _ObjMeshFaceIndex face_index;
        char interupt;
        for (int i = 0; i < 3; ++i)
        {
          str_stream >> face_index.pos_index[i] >> interupt
                       >> face_index.tex_index[i]  >> interupt
                       >> face_index.nor_index[i];
        }
        faces.push_back(face_index);
      }
    }
    // Explicit closing of the file
    filestream.close();

    nux::MeshData* md = new nux::MeshData;
    md->Allocate(faces.size(), nux::NUX_MESH_TRIANGLE, 3*faces.size(), 16 + 12 + 8);

    //md->_mesh_primitive_type = nux::NuxMeshPrimitiveType::NUX_MESH_TRIANGLE;

    size_t i = 0;

    float* vertex_buffer = (float*)md->_vertex_data;
//     for (i = 0; i < positions.size(); i++)
//     {
//       vertex_buffer[9*i + 0] = positions[i].x;
//       vertex_buffer[9*i + 1] = positions[i].y;
//       vertex_buffer[9*i + 2] = positions[i].z;
//       vertex_buffer[9*i + 3] = positions[i].w;
// 
//       vertex_buffer[9*i + 4] = normals[i].x;
//       vertex_buffer[9*i + 5] = normals[i].y;
//       vertex_buffer[9*i + 6] = normals[i].z;
// 
//       vertex_buffer[9*i + 7] = texcoords[i].x;
//       vertex_buffer[9*i + 8] = texcoords[i].y;
//     }

    int* index_buffer = (int*)md->_index_data;
    for (i = 0; i < faces.size(); i++)
    {
      index_buffer[3*i + 0] = 3*i + 0;  //faces[i].pos_index[0]-1;
      index_buffer[3*i + 1] = 3*i + 1;  //faces[i].pos_index[1]-1;
      index_buffer[3*i + 2] = 3*i + 2;  //faces[i].pos_index[2]-1;

      for (int j = 0; j < 3; j++)
      {
        int vi    = faces[i].pos_index[j]-1;
        int ni    = faces[i].nor_index[j]-1;
        int ti    = faces[i].tex_index[j]-1;

        vertex_buffer[27*i + 9*j + 0] = positions[vi].x;
        vertex_buffer[27*i + 9*j + 1] = positions[vi].y;
        vertex_buffer[27*i + 9*j + 2] = positions[vi].z;
        vertex_buffer[27*i + 9*j + 3] = positions[vi].w;

        vertex_buffer[27*i + 9*j + 4] = normals[ni].x;
        vertex_buffer[27*i + 9*j + 5] = normals[ni].y;
        vertex_buffer[27*i + 9*j + 6] = normals[ni].z;

        vertex_buffer[27*i + 9*j + 7] = texcoords[ti].x;
        vertex_buffer[27*i + 9*j + 8] = texcoords[ti].y;
      }
    }

    return md;
  }
}
