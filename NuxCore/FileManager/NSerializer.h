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


#ifndef NSERIALIZER_H
#define NSERIALIZER_H

namespace nux   //NUX_NAMESPACE_BEGIN
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
    virtual void SerializeFinal (void *V, t_s64 Length) = 0;
//     template<typename T>
//     void SerializeBuffer( T* buffer, t_u64 NumberOfElements, t_u64 ElementSize = sizeof(T))
//     {
//         for(t_u64 i = 0; i < NumberOfElements; i++)
//         {
//             t_u8* bytebuffer = (t_u8*)(&buffer[i]);
//             Serialize(bytebuffer, ElementSize);
//         }
//     }
    virtual bool isReader() = 0;
    virtual bool isWriter() = 0;
    virtual t_s64 Tell() = 0;
    virtual t_s64 GetFileSize()
    {
      return -1;
    }
    virtual t_s64 Seek (t_s64 FilePos, NSerializer::SeekPos) = 0;
    virtual bool Precache (INT PrecacheOffset, INT PrecacheSize)
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
          Serialize ( (t_u8 *) V + i, 1 );
      }
      else
      {
        // Transferring around within memory, so keep the byte order.
        Serialize ( (t_u8 *) V, Length);
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

    virtual void Serialize (t_char   &data);
    virtual void Serialize (t_wchar  &data);
    virtual void Serialize (t_bool   &data);
    virtual void Serialize (t_s8     &data);
    virtual void Serialize (t_u8     &data);
    virtual void Serialize (t_u16    &data);
    virtual void Serialize (t_s16    &data);
    virtual void Serialize (t_uint32 &data);
    virtual void Serialize (t_int32  &data);
    virtual void Serialize (t_long   &data);
    virtual void Serialize (t_ulong  &data);
    virtual void Serialize (t_float  &data);
    virtual void Serialize (t_double &data);
    virtual void Serialize (t_u64    &data);
    virtual void Serialize (t_s64    &data);

    virtual void Serialize (t_char   *buffer, t_u32 len, t_u32 stride = sizeof (t_char) );
    virtual void Serialize (t_wchar  *buffer, t_u32 len, t_u32 stride = sizeof (t_wchar) );
    virtual void Serialize (t_bool   *buffer, t_u32 len, t_u32 stride = sizeof (t_bool)  );
    virtual void Serialize (t_s8     *buffer, t_u32 len, t_u32 stride = sizeof (t_s8)    );
    virtual void Serialize (t_u8     *buffer, t_u32 len, t_u32 stride = sizeof (t_u8)    );
    virtual void Serialize (t_u16    *buffer, t_u32 len, t_u32 stride = sizeof (t_u16)   );
    virtual void Serialize (t_s16    *buffer, t_u32 len, t_u32 stride = sizeof (t_s16)   );
    virtual void Serialize (t_uint32 *buffer, t_u32 len, t_u32 stride = sizeof (t_uint32) );
    virtual void Serialize (t_int32  *buffer, t_u32 len, t_u32 stride = sizeof (t_int32) );
    virtual void Serialize (t_long   *buffer, t_u32 len, t_u32 stride = sizeof (t_long)  );
    virtual void Serialize (t_ulong  *buffer, t_u32 len, t_u32 stride = sizeof (t_ulong) );
    virtual void Serialize (t_float  *buffer, t_u32 len, t_u32 stride = sizeof (t_float) );
    virtual void Serialize (t_double *buffer, t_u32 len, t_u32 stride = sizeof (t_double) );
    virtual void Serialize (t_u64    *buffer, t_u32 len, t_u32 stride = sizeof (t_u64)   );
    virtual void Serialize (t_s64    *buffer, t_u32 len, t_u32 stride = sizeof (t_s64)   );

    virtual void Identify (const char *name) {};
    virtual void Begin() {};
    virtual void End() {};

  protected:
    void Reset (void)
    {
      m_ErrorCode = FALSE;
    }
    bool m_ErrorCode;
  };

  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_char     &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_bool      &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_s8        &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_u8        &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_u16       &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_s16       &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_uint32    &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_int32     &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_long      &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_ulong     &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_float     &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_double    &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_u64       &v)
  {
    Sr.Serialize (v);
    return Sr;
  }
  NUX_INLINE NSerializer &operator << (NSerializer &Sr, t_s64       &v)
  {
    Sr.Serialize (v);
    return Sr;
  }


} //NUX_NAMESPACE_END

#endif // NSERIALIZER_H

