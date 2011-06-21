#ifndef MESHDATA_H
#define MESHDATA_H

namespace nux
{
  class MeshData
  {
  public:
    MeshData();
    ~MeshData();

    bool Allocate(int num_index, int index_size, int num_elements, int element_size);

  public:
    unsigned char* _vertex_data;
    unsigned char* _index_data;
    int   _num_element;
    int   _element_size;
    int   _num_index;
    int   _index_size;
  };
}

#endif // MESHDATA_H
