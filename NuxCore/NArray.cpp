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
void NDataArray::Realloc(t_u32 NewReservedSize, t_u32 ElementSize, t_u32 Alignment )
{
    if(m_ReservedSize < NewReservedSize)
    {
        void* NewData = ::malloc(NewReservedSize*ElementSize);
        nuxAssertMsg(NewData != 0, TEXT("[NDataArray::Realloc] Out of Memory!"));

        if(NewData)
        {
            if(m_ArraySize)
                Memcpy(NewData, m_Data, m_ArraySize*ElementSize);
        }
        if(m_Data)
            ::free(m_Data);
        m_Data = (BYTE*)NewData;
        m_ReservedSize = NewReservedSize;
    }
}


void NDataArray::Remove(t_u32 index, t_u32 count, t_u32 size, t_u32 alignment)
{
    nuxAssert(count >= 0);
    nuxAssert(index >= 0); 
    nuxAssert(index <= m_ArraySize);
    nuxAssert(index + count <= m_ArraySize);

    Memmove
        (
        (BYTE*)m_Data + (index      ) * size,
        (BYTE*)m_Data + (index+count) * size,
        (m_ArraySize - index - count ) * size
        );
    m_ArraySize -= count;

    nuxAssert(m_ArraySize >= 0);
    nuxAssert(m_ReservedSize >= m_ArraySize);
}

NAMESPACE_END
