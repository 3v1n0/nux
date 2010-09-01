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
#include "NUniqueIndex.h"

NAMESPACE_BEGIN

INL_IMPLEMENT_GLOBAL_OBJECT(NUniqueIndex);

NCriticalSection NUniqueIndex::m_CriticalSection;

void NUniqueIndex::Constructor()
{
}

void NUniqueIndex::Destructor()
{
}

t_u64 NUniqueIndex::GetUniqueIndex()
{
    NScopeLock Scope(&m_CriticalSection);
    m_UniqueIndex.Increment();
    return m_UniqueIndex.GetValue();
}

NAMESPACE_END

