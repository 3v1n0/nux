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


#ifndef NPNG_H
#define NPNG_H

#include "NuxCore/Error.h"
#include "ImageSurface.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

#ifdef WIN32
  void set_png_module_handle (unsigned long hM);
  void set_png_module_restypename (const char *tname);
#endif

  NBitmapData *read_png_rgb (const TCHAR *filename);
  NBitmapData *read_png_rgba (const TCHAR *filename);

} //NUX_NAMESPACE_END

#endif // NPNG_H

