/*
 * Copyright (C) 2012 - Canonical Ltd.
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
 * Authored by: Daniel d'Andrada <daniel.dandrada@canonical.com>
 */

#ifndef DAN_TESTBUTTON_H
#define DAN_TESTBUTTON_H

#include <Nux/Button.h>
#include <Nux/PaintLayer.h>

class TestButton : public nux::Button
{
  public:
    TestButton(const std::string& button_label, NUX_FILE_LINE_PROTO);
    virtual ~TestButton();

  protected:
    virtual void Draw(nux::GraphicsEngine &graphics_engine, bool force_draw);
};

#endif
