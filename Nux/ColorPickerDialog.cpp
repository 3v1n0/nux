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


#include "Nux.h"
#include "Button.h"
#include "ColorPickerDialog.h"

namespace nux
{

  static Color DummyColor;

  ColorPickerDialog::ColorPickerDialog()
    :   m_Color(DummyColor)
  {
    SetWindowTitle("Color Picker");
    SetWindowSizeMatchLayout(true);
    m_Vlayout = new VLayout("Color Picker");
    m_ColorEditor = new ColorEditor();

    m_Vlayout->AddView(m_ColorEditor);
    m_Vlayout->AddLayout(m_ButtonLayout);

    m_Vlayout->SetScaleFactor(0);
    // Set layout for the window
    SetLayout(m_Vlayout);
  }

  ColorPickerDialog::~ColorPickerDialog()
  {

  }

  void ColorPickerDialog::StartDialog(Color &color, color::Model /* ColorModel */, bool StartModal)
  {
    Dialog::Start(StartModal);
    m_Color = color;
  }

  void ColorPickerDialog::RecvOk()
  {
    m_Color = m_ColorEditor->GetRGBColor();
    m_Color = color::Black;
    ShowWindow(FALSE);
  }

  void ColorPickerDialog::RecvCancel()
  {
    m_Color = DummyColor;
    ShowWindow(FALSE);
  }


}
