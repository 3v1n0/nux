#ifndef MESHDATA_H
#define MESHDATA_H

namespace nux
{
  enum NuxMeshPrimitiveType
  {
    NUX_MESH_PRIMITIVE_UNKNOWN = 0,
    NUX_MESH_TRIANGLE,
    NUX_MESH_TRIANGLE_STRIP,
    NUX_MESH_TRIANGLE_FAN,
    NUX_MESH_LINE,
    NUX_MESH_LINE_STRIP,
    NUX_MESH_LINE_LOOP,
    NUX_MESH_POINT,
    NUX_MESH_QUAD,
    NUX_MESH_QUAD_STRIP,
  };

  class MeshData
  {
  public:
    MeshData();
    ~MeshData();

    bool Allocate(int num_mesh_primitive, NuxMeshPrimitiveType primitive_type, int num_elements, int element_size);

  public:
    unsigned char* _vertex_data;
    unsigned char* _index_data;
    NuxMeshPrimitiveType _mesh_primitive_type;
    int   _num_element;
    int   _element_size;
    int   _num_index;
    int   _index_size;
  };
}

#endif // MESHDATA_H
