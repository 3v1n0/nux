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


#ifndef NSERIALIZER_H
#define NSERIALIZER_H

namespace nux
{

  class NSerializer
  {
  public:
    enum
    {
      OutputErrorIfFail   = 0x01,
      NoOverWrite         = 0x02,
      OverWriteReadOnly   = 0x04,
      Unbuffered          = 0x08,
      Append			    = 0x10,
      Read                = 0x20,
      Write               = 0x40,
    };

    typedef enum
    {
      SeekStart           = 0x00,
      SeekCurrent         = 0x01,
      SeekEnd             = 0x02,
    } SeekPos;

    // NSerializer interface.
    virtual ~NSerializer() {}
    virtual void SerializeFinal (void *V, long long Length) = 0;
//     template<typename T>
//     void SerializeBuffer( T* buffer, unsigned long long NumberOfElements, unsigned long long ElementSize = sizeof(T))
//     {
//         for(unsigned long long i = 0; i < NumberOfElements; i++)
//         {
//             unsigned char* bytebuffer = (unsigned char*)(&buffer[i]);
//             Serialize(bytebuffer, ElementSize);
//         }
//     }
    virtual bool isReader() = 0;
    virtual bool isWriter() = 0;
    virtual long long Tell() = 0;
    virtual long long GetFileSize()
    {
      return -1;
    }
    virtual long long Seek (long long FilePos, NSerializer::SeekPos) = 0;
    virtual bool Precache (INT /* PrecacheOffset */, INT /* PrecacheSize */)
    {
      return TRUE;
    }
    virtual void Flush() {};
    virtual bool Close() = 0;
    virtual bool GetError()
    {
      return m_ErrorCode;
    }

    NSerializer &ByteOrderSerialize (void *V, INT Length )
    {
      BOOL SwapBytes = 0;

      if ( SwapBytes )
      {
        // Transferring between memory and file, so flip the byte order.
        for ( INT i = Length - 1; i >= 0; i-- )
          Serialize ( (unsigned char *) V + i, 1 );
      }
      else
      {
        // Transferring around within memory, so keep the byte order.
        Serialize ( (unsigned char *) V, Length);
      }

      return *this;
    }

    // Constructor.
    NSerializer()
    {
      Reset();
    }

    NUX_INLINE bool IsError() const
    {
      return m_ErrorCode;
    }

    virtual void Serialize (char   &data);
    virtual void Serialize (wchar_t  &data);
    virtual void Serialize (bool   &data);
    virtual void Serialize (unsigned char     &data);
    virtual void Serialize (unsigned short    &data);
    virtual void Serialize (short    &data);
    virtual void Serialize (unsigned int &data);
    virtual void Serialize (int  &data);
    virtual void Serialize (long   &data);
    virtual void Serialize (unsigned long  &data);
    virtual void Serialize (float  &data);
    virtual void Serialize (double &data);
    virtual void Serialize (unsigned long long    &data);
    virtual void Serialize (long long    &data);

    virtual void Serialize (wchar_t  *buffer, unsigned int len, unsigned int stride = sizeof (wchar_t) );
    virtual void Serialize (bool   *buffer, unsigned int len, unsigned int stride = sizeof (bool)  );
    virtual void Serialize (char   *buffer, unsigned int len, unsigned int stride = sizeof (char) );
    virtual void Serialize (unsigned char     *buffer, unsigned int len, unsigned int stride = sizeof (unsigned char)    );
    virtual void Serialize (unsigned short    *buffer, unsigned int len, unsigned int stride = sizeof (unsigned short)   );
    virtual void Serialize (short    *buffer, unsigned int len, unsigned int stride = sizeof (short)   );
    virtual void Serialize (unsigned int *buffer, unsigned int len, unsigned int stride = sizeof (unsigned int) );
    virtual void Serialize (int  *buffer, unsigned int len, unsigned int stride = sizeof (int) );
    virtual void Serialize (long   *buffer, unsigned int len, unsigned int stride = sizeof (long)  );
    virtual void Serialize (unsigned long  *buffer, unsigned int len, unsigned int stride = sizeof (unsigned long) );
    virtual void Serialize (float  *buffer, unsigned int len, unsigned int stride = sizeof (float) );
    virtual void Serialize (double *buffer, unsigned int len, unsigned int stride = sizeof (double) );
    virtual void Serialize (unsigned long long    *buffer, unsigned int len, unsigned int stride = sizeof (unsigned long long)   );
    virtual void Serialize (long long    *buffer, unsigned int len, unsigned int stride = sizeof (long long)   );

    virtual void Identify (const char* /* name */) {};
    virtual void Begin() {};
    virtual void End() {};

  protected:
    void Reset (void)
    {
      m_ErrorCode = FALSE;
    }
    bool m_ErrorCode;
  };

  NUX_INLINE NSerializer &operator << (NSerializer &Sr, bool      &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, char     &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, unsigned char        &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, unsigned short       &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, short       &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, unsigned int    &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, int     &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, long      &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, unsigned long     &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, float     &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, double    &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, unsigned long long       &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, long long       &v)
  {
    Sr.Serialize (v);
    return Sr;
  }


}

#endif // NSERIALIZER_H

