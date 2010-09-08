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


#include "Nux.h"
#include "NuxGlobalInitializer.h"

NAMESPACE_BEGIN

static NuxGlobalSingletonInitializer* GNuxGlobalInitializer   = 0;

static void SystemStart()
{
    static t_u8 StaticBuffer[sizeof(NuxGlobalSingletonInitializer)];
    // Placement new in our reserved buffer.
    GNuxGlobalInitializer =  new(StaticBuffer) NuxGlobalSingletonInitializer();

    //GLogDevice.AddOutputDevice( &NUX_GLOBAL_OBJECT_INSTANCE(NOutputLogFile) );
    //GLogDevice.AddOutputDevice( &NUX_GLOBAL_OBJECT_INSTANCE(NOutputVisualDebugConsole) );
}

static void SystemShutdown()
{
    // Manually destroy initializer
    if(GNuxGlobalInitializer)
        GNuxGlobalInitializer->~NuxGlobalSingletonInitializer();
    GNuxGlobalInitializer = 0;

}

bool NuxGlobalSingletonInitializer::m_NuxGlobalObjectsReady = false;
NuxGlobalSingletonInitializer::NuxGlobalSingletonInitializer()
{
    m_NuxGlobalObjectsReady = true;
}

NuxGlobalSingletonInitializer::~NuxGlobalSingletonInitializer()
{
    m_NuxGlobalObjectsReady = false;
}

bool NuxGlobalSingletonInitializer::Ready()
{
    return m_NuxGlobalObjectsReady;
}

int NuxGlobalInitializer::m_Count = 0;
NuxGlobalInitializer::NuxGlobalInitializer()
{
    if(m_Count++ == 0)
    {
        SystemStart();
    }
}

NuxGlobalInitializer::~NuxGlobalInitializer()
{
    if(--m_Count == 0)
    {
        SystemShutdown();
    }
}

NAMESPACE_END
