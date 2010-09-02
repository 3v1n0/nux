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


#include "../NKernel.h"
#include "NRefCount.h"

NAMESPACE_BEGIN

NCriticalSection NRefCount::m_CriticalSection;

NRefCount::NRefCount()
{

}

NRefCount::~NRefCount()
{

}

void NRefCount::Increment()
{
    NScopeLock Scope(&m_CriticalSection);
    m_TSCounter.Increment();
}

void NRefCount::Decrement()
{
    NScopeLock Scope(&m_CriticalSection);
    m_TSCounter.Decrement();
}

t_u32 NRefCount::GetRefCount() const
{
    NScopeLock Scope(&m_CriticalSection);
    return m_TSCounter.GetValue();
}

NAMESPACE_END
