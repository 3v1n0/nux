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
 * Authored by: Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */


#include "NuxCore.h"
#include "InitiallyUnownedObject.h"
#include "ObjectPtr.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (InitiallyUnownedObject);

  InitiallyUnownedObject::InitiallyUnownedObject (NUX_FILE_LINE_DECL)
    : Object (false, NUX_FILE_LINE_PARAM)
  {
    _allocation_file_name      = __Nux_FileName__;
    m_allocation_line_number    = __Nux_LineNumber__;
  }

  InitiallyUnownedObject::~InitiallyUnownedObject()
  {
  }

}
