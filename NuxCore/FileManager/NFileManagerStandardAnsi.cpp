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
#include "Math/MathUtility.h"

namespace nux
{

// Choose the size so it is a power of 2. Example (size-1)= 11111111.
  const t_int NSerializeFileReader::sBufferSize = 1024;

  NSerializeFileReader::NSerializeFileReader (FILE *InFile, LogOutputDevice *InError, t_int InSize)
    : m_File (InFile)
    , m_Error (InError)
    , m_FileSize (InSize)
    , m_FilePos (0)
    , BufferBase (0)
    , BufferCount (0)
  {
    m_PreCacheBuffer = new BYTE[sBufferSize];
    fseek (m_File, 0, SEEK_SET);
  }

  NSerializeFileReader::~NSerializeFileReader()
  {
    delete [] m_PreCacheBuffer;

    if (m_File)
      Close();
  }

  bool NSerializeFileReader::Precache (t_int PrecacheOffset, t_int PrecacheSize)
  {
    // Only precache at current position and avoid work if precaching same offset twice.
    if ( (m_FilePos == PrecacheOffset) && (!BufferBase || !BufferCount || (BufferBase != m_FilePos) ) )
    {
      BufferBase = m_FilePos;
      BufferCount = Min (Min (PrecacheSize, (t_int) (sBufferSize - (m_FilePos & (sBufferSize - 1) ) ) )
                         , m_FileSize - m_FilePos);

      //GTotalBytesReadViaFileManager += BufferCount;
      if ( (::fread (m_PreCacheBuffer, BufferCount, 1, m_File) != BufferCount) && (PrecacheSize != 0) )
      {
        m_ErrorCode = 1;

        if (feof (m_File) )
        {
          m_Error->LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("[NSerializeFileReader::Precache] Error: fread reached end of file") );
        }
        else
        {
          m_Error->LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("[NSerializeFileReader::Precache] Error: fread failed: (ferror = %i)"), ferror (m_File) );
        }
      }
    }

    return TRUE;
  }

  t_s64 NSerializeFileReader::Seek (t_s64 InPos, NSerializer::SeekPos seekpos)
  {
    nuxAssert (InPos >= 0);
    nuxAssert (InPos <= m_FileSize);

    if (::fseek (m_File, InPos, SEEK_SET) )
    {
      m_ErrorCode = 1;
      nuxAssertMsg (0, TEXT ("[NSerializeFileReader::Seek] Error: fseek failed: (ferror = %i)"), ferror (m_File) );
    }

    m_FilePos   = InPos;
    BufferBase  = m_FilePos;
    BufferCount = 0;

    return InPos;
  }

  t_s64 NSerializeFileReader::Tell()
  {
    return m_FilePos;
  }

  t_s64 NSerializeFileReader::GetFileSize()
  {
    return m_FileSize;
  }

  bool NSerializeFileReader::Close()
  {
    if (m_File)
      ::fclose (m_File);

    m_File = NULL;
    return !m_ErrorCode;
  }

  void NSerializeFileReader::SerializeFinal (void *Dest, t_s64 Length)
  {
    while (Length > 0)
    {
      t_int Copy = Min<t_s64> (Length, BufferBase + BufferCount - m_FilePos);

      if (Copy == 0)
      {
        if (Length >= sBufferSize)
        {
          // GTotalBytesReadViaFileManager += Length;
          if (::fread (Dest, Length, 1, m_File) != Length)
          {
            m_ErrorCode = 1;
            m_Error->LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("[NSerializeFileReader::Serialize] Error: fread failed: (ferror = %i)"), ferror (m_File) );
          }

          m_FilePos   += Length;
          BufferBase  += Length;
          return;
        }

        Precache (m_FilePos, t_s32_max);
        Copy = Min<t_s64> (Length, BufferBase + BufferCount - m_FilePos);

        if (Copy <= 0)
        {
          m_ErrorCode = 1;
          m_Error->LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("[NSerializeFileReader::Serialize] Error: fread reached end of file: (ferror = %i)"), ferror (m_File) );
        }

        if (m_ErrorCode)
          return;
      }

      Memcpy (Dest, m_PreCacheBuffer + m_FilePos - BufferBase, Copy);
      m_FilePos   += Copy;
      Length      -= Copy;
      Dest        = (BYTE *) Dest + Copy;
    }
  }

//////////////////////////////////////////////////////////////////////////
  const t_int NSerializeFileWriter::sBufferSize = 4096;

  NSerializeFileWriter::NSerializeFileWriter (FILE *InFile, LogOutputDevice *InError)
    : m_File (InFile)
    , m_Error (InError)
    , m_FilePos (0)
    , m_CachePos (0)
  {
    m_CacheBuffer = new BYTE[sBufferSize];
  }

  NSerializeFileWriter::~NSerializeFileWriter()
  {
    NUX_SAFE_DELETE_ARRAY (m_CacheBuffer);

    if (m_File)
      Close();

    m_File = NULL;
  }

  t_s64 NSerializeFileWriter::Seek (t_s64 InPos, NSerializer::SeekPos seekpos)
  {
    Flush();

    if (::fseek (m_File, InPos, SEEK_SET) )
    {
      m_ErrorCode = 1;
      m_Error->LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("[NSerializeFileWriter::Seek] Seek Failed.") );
    }

    m_FilePos = InPos;

    return InPos;
  }

  t_s64 NSerializeFileWriter::Tell()
  {
    return m_FilePos;
  }

  t_s64 NSerializeFileWriter::GetFileSize()
  {

#if defined(NUX_VISUAL_STUDIO_2005) || defined(NUX_VISUAL_STUDIO_2008)
    t_s64 CurrentPosition = _ftelli64 (m_File);
    _fseeki64 (m_File, 0, SEEK_END);
    t_s64 FileSize = _ftelli64 (m_File);
    _fseeki64 (m_File, CurrentPosition, SEEK_SET);
#else
    t_s32 CurrentPosition = ftell (m_File);
    fseek (m_File, 0, SEEK_END);
    t_s32 FileSize = ftell (m_File);
    fseek (m_File, CurrentPosition, SEEK_SET);
#endif
    return FileSize;
  }

  bool NSerializeFileWriter::Close()
  {
    Flush();

    if (m_File && ::fclose (m_File) )
    {
      m_ErrorCode = 1;
      m_Error->LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("[NSerializeFileWriter::Close] Write Failed.") );
    }

    return !m_ErrorCode;
  }

  void NSerializeFileWriter::SerializeFinal (void *Src, t_s64 Length)
  {
    BYTE *SrcBuffer = NUX_STATIC_CAST (BYTE *, Src);
    m_FilePos += Length;
    t_int Copy;

    while (Length > (Copy = sBufferSize - m_CachePos) )
    {
      // There is not enough room to write Length Bytes. Write as much as you can and Flush.
      Memcpy (m_CacheBuffer + m_CachePos, SrcBuffer, Copy);
      m_CachePos  += Copy;
      Length      -= Copy;
      SrcBuffer   = (BYTE *) SrcBuffer + Copy;
      m_FilePos   += Copy;
      Flush();
    }

    if (Length)
    {
      // Write the data that remain in the cache
      Memcpy (m_CacheBuffer + m_CachePos, SrcBuffer, Length);
      m_CachePos += Length;
      m_FilePos += Length;
    }
  }

  void NSerializeFileWriter::Flush()
  {
    //GTotalBytesWrittenViaFileManager += m_CachePos;
    if (m_CachePos && ::fwrite (m_CacheBuffer, m_CachePos, 1, m_File) != 1)
    {
      m_ErrorCode = 1;
      m_Error->LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("[NSerializeFileWriter::Flush] Write Failed.") );
    }

    m_CachePos = 0;
  }

}

