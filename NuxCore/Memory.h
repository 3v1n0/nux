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


#ifndef NMEMORY_H
#define NMEMORY_H

#define NUX_DEFAULT_ALIGNMENT 4

#define NUX_SAFE_DELETE(mem)    if(mem)             \
                                {                   \
                                    delete(mem);    \
                                    (mem) = 0;      \
                                }
#define NUX_SAFE_DELETE_ARRAY(mem_array)    if(mem_array)               \
                                            {                           \
                                                delete[](mem_array);    \
                                                (mem_array) = 0;        \
                                            }


namespace nux
{

  unsigned int Memcmp ( const void *Buf1, const void *Buf2, unsigned int Count );

  bool MemIsZero ( const void *V, size_t Count );

  void *Memmove ( void *Dest, const void *Src, size_t Count );

  void Memset ( void *Dest, int C, size_t Count );

  void Memzero ( void *Dest, size_t Count );

  void Memcpy ( void *Dest, const void *Src, size_t Count );

  void Memswap ( void *Ptr1, void *Ptr2, size_t Size );

//! Check that the alignment is a power of two
  bool IsMemoryAligned (void *data, unsigned int alignment);

  void *Malloc (size_t Count, unsigned int Alignment = NUX_DEFAULT_ALIGNMENT);
  void *Realloc (void *Original, size_t Count, unsigned int Alignment = NUX_DEFAULT_ALIGNMENT);

}

#endif // NMEMORY_H
