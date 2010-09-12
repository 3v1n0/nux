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

static NGlobalSingletonInitializer*      GGlobalInitializer   = 0;

static void SystemStart()
{
    static t_u8 StaticBuffer[sizeof(NGlobalSingletonInitializer)];
    // Placement new in our reserved buffer.
    GGlobalInitializer =  new(StaticBuffer) NGlobalSingletonInitializer();

    GLogDevice.AddOutputDevice( &NUX_GLOBAL_OBJECT_INSTANCE(NOutputLogFile) );
    GLogDevice.AddOutputDevice( &NUX_GLOBAL_OBJECT_INSTANCE(NOutputVisualDebugConsole) );
}

static void SystemShutdown()
{
    // Manually destroy initializer
    if(GGlobalInitializer)
        GGlobalInitializer->~NGlobalSingletonInitializer();
    GGlobalInitializer = 0;

}

bool NGlobalSingletonInitializer::m_bGlobalObjectsReady = false;
NGlobalSingletonInitializer::NGlobalSingletonInitializer()
{
    m_bGlobalObjectsReady = true;
}

NGlobalSingletonInitializer::~NGlobalSingletonInitializer()
{
    m_bGlobalObjectsReady = false;
}

bool NGlobalSingletonInitializer::Ready()
{
    return m_bGlobalObjectsReady;
}

int NGlobalInitializer::m_Count = 0;
NGlobalInitializer::NGlobalInitializer()
{
    if(m_Count++ == 0)
    {
        SystemStart();
    }
}

NGlobalInitializer::~NGlobalInitializer()
{
    if(--m_Count == 0)
    {
        SystemShutdown();
    }
}

NAMESPACE_END
