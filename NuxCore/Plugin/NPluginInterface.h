/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef NPLUGININTERFACE_H
#define NPLUGININTERFACE_H

#include "NPlugin.h"

namespace nux
{

  class NPluginInterface
  {
  public:
    NPluginInterface() {}
    virtual ~NPluginInterface() {}

    virtual int Activate() = 0;
    virtual int Execute() = 0;
    virtual int Destroy() = 0;
  };


}

#endif // NPLUGININTERFACE_H


