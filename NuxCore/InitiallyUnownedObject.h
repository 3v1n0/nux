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

#include "Object.h"

#ifndef NUXINITIALLYUNOWNEDOBJECT_H
#define NUXINITIALLYUNOWNEDOBJECT_H

namespace nux
{

//! The base class of Nux initially unowned objects.
  class InitiallyUnownedObject: public Object
  {
  public:
    NUX_DECLARE_OBJECT_TYPE (InitiallyUnownedObject, Object);

    //! Constructor
    InitiallyUnownedObject (NUX_FILE_LINE_PROTO);
    ~InitiallyUnownedObject ();

  private:
    NString m_allocation_file_name;
    int     m_allocation_line_number;
  };

}

#endif // NUXINITIALLYUNOWNEDOBJECT_H
