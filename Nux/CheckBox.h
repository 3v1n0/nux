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


#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "AbstractButton.h"

namespace nux
{
  class CheckBox: public AbstractButton
  {
  public:
    CheckBox (std::string label, NUX_FILE_LINE_PROTO);
    ~CheckBox();

    Property<std::string>   label;

  private:
    void Init ();

    void OnStateChanged (int value);
    void OnLabelChanged (std::string value);
    void RebuildLayout ();

    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);

  protected:
    virtual Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);

  private:
  };
}

#endif // CHECKBOX_H
