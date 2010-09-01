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

#include "NMemoryHook.h"

NAMESPACE_BEGIN

INL_IMPLEMENT_GLOBAL_OBJECT(MemHook)

// Singleton constructor 
void MemHook::Constructor()
{
    // The first hook will point to our default hook...
    m_pRootHook = &m_DefaultHook;
}

// Singleton destructor 
void MemHook::Destructor()
{
    // Check if all object that has hooked unregistered like they should...
    nuxAssert(m_pRootHook == &m_DefaultHook);
}    	

void MemoryHook::Hook()
{
    // Register 
    m_pNextHook = INL_GLOBAL_OBJECT_INSTANCE(MemHook).Hook(this);
}

void MemoryHook::UnHook()
{
    // Unregister 
    INL_GLOBAL_OBJECT_INSTANCE(MemHook).UnHook(this, m_pNextHook);
    m_pNextHook = 0;
}

NAMESPACE_END

