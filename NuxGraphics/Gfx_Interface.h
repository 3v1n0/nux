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


#ifndef GRAPHICINTERFACE_H
#define GRAPHICINTERFACE_H

#if defined(INL_OS_WINDOWS)
    #include "Gfx_Events.h"
#elif defined(INL_OS_LINUX)
    #include "GfxEventsX11.h"
#else
    #error Undefined GfxEvents for this platform
#endif

NAMESPACE_BEGIN_OGL

typedef struct _DeviceModes
{
    int width;
    int height;
    int format;
    bool fullscreen;
    int refresh_rate;
} DeviceModes;

class GraphicSystem
{
public:
    int m_num_gfx_device_modes;
    std::vector<DeviceModes> m_gfx_device_modes;
};

NAMESPACE_END_OGL

#endif //GRAPHICINTERFACE_H