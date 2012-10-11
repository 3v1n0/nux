#include "GLResource.h"
#include "MeshData.h"


namespace nux
{
  MeshData::MeshData()
  {
    _num_index = 0;
    _index_size = 0;
    _num_element = 0;
    _vertex_data = 0;
    _index_data = 0;
  }

  MeshData::~MeshData()
  {
    NUX_SAFE_DELETE_ARRAY(_vertex_data);
    NUX_SAFE_DELETE_ARRAY(_index_data);
  }

  bool MeshData::Allocate(int num_mesh_primitive, NuxMeshPrimitiveType primitive_type, int num_element, int element_size)
  {
    NUX_RETURN_VALUE_IF_FALSE(num_element > 0, false);
    NUX_RETURN_VALUE_IF_FALSE(element_size > 0, false);

    NUX_SAFE_DELETE_ARRAY(_vertex_data);
    NUX_SAFE_DELETE_ARRAY(_index_data);

    _num_element = num_element;
    _element_size = element_size;

    _mesh_primitive_type = primitive_type;
    switch(primitive_type)
    {
    case NUX_MESH_TRIANGLE:
      _num_index = num_mesh_primitive * 3;
      break;

    case NUX_MESH_TRIANGLE_STRIP:
    case NUX_MESH_TRIANGLE_FAN:
      _num_index = num_mesh_primitive + 2;
      break;

    case NUX_MESH_LINE:
        _num_index = num_mesh_primitive * 2;
      break;

    case NUX_MESH_LINE_STRIP:
    case NUX_MESH_LINE_LOOP:
      _num_index = num_mesh_primitive + 1;
      break;

    case NUX_MESH_QUAD:
      _num_index = num_mesh_primitive * 4;
      break;

    case NUX_MESH_QUAD_STRIP:
      _num_index = num_mesh_primitive + 2;
      break;

    case NUX_MESH_POINT:
      _num_index = num_mesh_primitive;
      break;
    case NUX_MESH_PRIMITIVE_UNKNOWN:
      return false;
      break;
    }
    _index_size = 4;

    _vertex_data = new unsigned char[num_element * element_size];
    _index_data = new unsigned char[_num_index * _index_size];

    return true;
  }

}
