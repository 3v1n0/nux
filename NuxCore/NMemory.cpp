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


#include "NKernel.h"

NAMESPACE_BEGIN

t_u32 Memcmp( const void* Buf1, const void* Buf2, t_u32 Count )
{
    return std::memcmp( Buf1, Buf2, Count );
}

bool MemIsZero( const void* V, t_size Count )
{
    t_u8* B = (t_u8*)V;
    while( Count-- > 0 )
        if( *B++ != 0 )
            return false;
    return true;
}

void* Memmove( void* Dest, const void* Src, t_size Count )
{
    if(Count == 0)
        return Dest;
    return std::memmove( Dest, Src, Count );
}

void Memset( void* Dest, t_s32 C, t_size Count )
{
    std::memset( Dest, C, Count );
}

void Memzero( void* Dest, t_size Count )
{
    std::memset( Dest, 0, Count );
}

void Memcpy( void* Dest, const void* Src, t_size Count )
{
    std::memcpy( Dest, Src, Count );
}

void Memswap( void* Ptr1, void* Ptr2, t_size Size )
{
    void* Temp = malloc(Size);
    Memcpy( Temp, Ptr1, Size );
    Memcpy( Ptr1, Ptr2, Size );
    Memcpy( Ptr2, Temp, Size );
    free(Temp);
}

bool IsMemoryAligned(void* data, t_u32 alignment)
{    
    nuxAssertMsg((alignment & (alignment-1)) == 0, TEXT("[IsMemAligned] Argument for memory alignment test is not a power of two: %d"), alignment); 
    return (((uintptr_t)&data) & (alignment-1)) == 0;
}

void* Malloc(t_size Count, t_u32 Alignment)
{ 
    return std::malloc( Count );
}

void* Realloc(void* Original, t_size Count, t_u32 Alignment)
{ 
    void* mem = std::realloc( Original, Count );
    nuxAssertMsg(mem, TEXT("[inlRealloc] realloc failed."));
    if(mem == 0)
        std::exit(0);
    return mem;
}

NAMESPACE_END
