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
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "NuxCore.h"

namespace nux
{

// template<typename T>
// void NSerializer::SerializeBuffer(T* buffer, unsigned long long Length, unsigned long long ElementSize)
// {
//     for(unsigned long long i = 0; i < Length; i++)
//     {
//         unsigned char* bytebuffer = (unsigned char*)(&buffer[i]);
//         Serialize(bytebuffer, ElementSize);
//     }
// }

// NSerializer& operator << (NSerializer& Sr, std::string& s)
// {
//     Sr.SerializeBuffer<TCHAR>(NUX_CONST_CAST(TCHAR*, s.GetTCharPtr()), s.Size()+1, sizeof(TCHAR));
//     return Sr;
// }
//
// NSerializer& operator << (NSerializer& Sr, std::string& s)
// {
//     Sr.SerializeBuffer<TCHAR>(NUX_CONST_CAST(char*, s.c_str()), s.size()+1, sizeof(char));
//     return Sr;
// }

  void NSerializer::Serialize (char   &data)
  {
    SerializeFinal (&data, sizeof (char)   );
  }

  void NSerializer::Serialize (wchar_t  &data)
  {
    SerializeFinal (&data, sizeof (wchar_t)   );
  }

  void NSerializer::Serialize (bool   &data)
  {
    SerializeFinal (&data, sizeof (bool)    );
  }

  void NSerializer::Serialize (unsigned char     &data)
  {
    SerializeFinal (&data, sizeof (unsigned char)      );
  }

  void NSerializer::Serialize (unsigned short    &data)
  {
    SerializeFinal (&data, sizeof (unsigned short)     );
  }
  void NSerializer::Serialize (short    &data)
  {
    SerializeFinal (&data, sizeof (short)     );
  }
  void NSerializer::Serialize (unsigned int &data)
  {
    SerializeFinal (&data, sizeof (unsigned int)  );
  }
  void NSerializer::Serialize (int  &data)
  {
    SerializeFinal (&data, sizeof (int)   );
  }
  void NSerializer::Serialize (long   &data)
  {
    SerializeFinal (&data, sizeof (long)    );
  }
  void NSerializer::Serialize (unsigned long  &data)
  {
    SerializeFinal (&data, sizeof (unsigned long)   );
  }
  void NSerializer::Serialize (float  &data)
  {
    SerializeFinal (&data, sizeof (float)   );
  }
  void NSerializer::Serialize (double &data)
  {
    SerializeFinal (&data, sizeof (double)  );
  }
  void NSerializer::Serialize (unsigned long long    &data)
  {
    SerializeFinal (&data, sizeof (unsigned long long)     );
  }
  void NSerializer::Serialize (long long    &data)
  {
    SerializeFinal (&data, sizeof (long long)     );
  }

  void NSerializer::Serialize (wchar_t  *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (bool   *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (char   *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (unsigned char     *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (unsigned short    *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (short    *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (unsigned int *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (int  *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (long   *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (unsigned long  *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (float  *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (double *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (unsigned long long    *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }
  void NSerializer::Serialize (long long    *buffer, unsigned int len, unsigned int stride)
  {
    SerializeFinal (buffer, len * stride);
  }

}
