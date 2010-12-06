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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef GRAPHICSDISPLAY_H
#define GRAPHICSDISPLAY_H

#if defined (NUX_OS_WINDOWS)
  #include "GraphicsDisplayWin.h"
#elif defined (NUX_OS_LINUX)
  #include "GraphicsDisplayX11.h"
#else
  #error "GraphicsDisplay.h" is not implemented for this platform.
#endif

#endif //GRAPHICSDISPLAY_H

