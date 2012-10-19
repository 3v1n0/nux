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

  unsigned int Memcmp ( const void *Buf1, const void *Buf2, unsigned int Count )
  {
    return std::memcmp ( Buf1, Buf2, Count );
  }

  bool MemIsZero ( const void *V, size_t Count )
  {
    unsigned char *B = (unsigned char *) V;

    while ( Count-- > 0 )
      if ( *B++ != 0 )
        return false;

    return true;
  }

  void *Memmove ( void *Dest, const void *Src, size_t Count )
  {
    if (Count == 0)
      return Dest;

    return std::memmove ( Dest, Src, Count );
  }

  void Memset ( void *Dest, int C, size_t Count )
  {
    std::memset ( Dest, C, Count );
  }

  void Memzero ( void *Dest, size_t Count )
  {
    std::memset ( Dest, 0, Count );
  }

  void Memcpy ( void *Dest, const void *Src, size_t Count )
  {
    std::memcpy ( Dest, Src, Count );
  }

  void Memswap ( void *Ptr1, void *Ptr2, size_t Size )
  {
    void *Temp = malloc (Size);
    Memcpy ( Temp, Ptr1, Size );
    Memcpy ( Ptr1, Ptr2, Size );
    Memcpy ( Ptr2, Temp, Size );
    free (Temp);
  }

  bool IsMemoryAligned (void *data, unsigned int alignment)
  {
    nuxAssertMsg ( (alignment & (alignment - 1) ) == 0, TEXT ("[IsMemAligned] Argument for memory alignment test is not a power of two: %d"), alignment);
    return ( ( (uintptr_t) &data) & (alignment - 1) ) == 0;
  }

  void *Malloc (size_t Count, unsigned int /* Alignment */)
  {
    return std::malloc ( Count );
  }

  void *Realloc (void *Original, size_t Count, unsigned int /* Alignment */)
  {
    void *mem = std::realloc ( Original, Count );

    if (mem == 0)
    {
      nuxCriticalMsg (TEXT ("[Realloc] realloc failed."));
      return NULL;
    }

    return mem;
  }

}
