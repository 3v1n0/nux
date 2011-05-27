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

  bool MeshData::Allocate(int num_index, int index_size, int num_element, int element_size)
  {
    NUX_RETURN_VALUE_IF_FALSE(num_index >0, false);
    NUX_RETURN_VALUE_IF_FALSE(index_size >0, false);
    NUX_RETURN_VALUE_IF_FALSE(num_element >0, false);
    NUX_RETURN_VALUE_IF_FALSE(element_size >0, false);

    NUX_SAFE_DELETE_ARRAY(_vertex_data);
    NUX_SAFE_DELETE_ARRAY(_index_data);

    _num_element = num_element;
    _element_size = element_size;
    _num_index = num_index;
    _index_size = index_size;

    _vertex_data = new unsigned char[num_element * element_size];
    _index_data = new unsigned char[num_index * index_size];

    return true;
  }

}
