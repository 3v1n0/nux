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


#ifndef COLORPICKERDIALOG_H
#define COLORPICKERDIALOG_H

#include "Dialog.h"
#include "ColorEditor.h"

namespace nux
{

  class HLayout;
  class VLayout;

  class ColorPickerDialog : public Dialog
  {
  public:
    ColorPickerDialog();
    ~ColorPickerDialog();

    virtual void RecvOk();
    virtual void RecvCancel();

    void StartDialog(Color &color, color::Model ColorModel = color::RGB, bool StartModal = false);

  private:

    VLayout        *m_Vlayout;
    ColorEditor    *m_ColorEditor;
    Color &m_Color;
  };

}

#endif // COLORPICKERDIALOG_H
