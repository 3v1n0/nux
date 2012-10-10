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


#ifndef HSEPARATOR_H
#define HSEPARATOR_H

#include "abstract-separator.h"

namespace nux
{

  class HSeparator: public AbstractSeparator
  {
  public:
    HSeparator();
    HSeparator (const Color &color, float Alpha0, float Alpha1, int Border);

    ~HSeparator();

  protected:
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine & /* GfxContext */, bool /* force_draw */) {};
  
  };

}

#endif // HSEPARATOR_H
