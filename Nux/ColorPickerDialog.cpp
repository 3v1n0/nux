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


#include "Nux.h"
#include "Button.h"
#include "ColorPickerDialog.h"

NAMESPACE_BEGIN_GUI

static Color DummyColor;

ColorPickerDialog::ColorPickerDialog()
:   m_Color(DummyColor)
{
    SetWindowTitle(TEXT("Color Picker"));
    SetWindowSizeMatchLayout(true);
    m_Vlayout = smptr(VLayout)(new VLayout(TEXT("Color Picker")));
    m_ColorEditor = smptr(ColorEditor)(new ColorEditor());

    m_Vlayout->AddActiveInterfaceObject(m_ColorEditor);
    m_Vlayout->AddLayout(m_ButtonLayout);

    m_Vlayout->SetStretchFactor(0);
    // Set layout for the window
    setLayout(m_Vlayout);
}


ColorPickerDialog::~ColorPickerDialog()
{
}

void ColorPickerDialog::StartDialog(Color& color, eColorModel ColorModel /*  = RGB */, bool StartModal /* = false */)
{
    Dialog::Start(StartModal);
    m_Color = color;
}

void ColorPickerDialog::RecvOk()
{
    m_Color = m_ColorEditor->GetRGBColor();
    m_Color = Color::Black;
    ShowWindow(FALSE);
}

void ColorPickerDialog::RecvCancel()
{
    m_Color = DummyColor;
    ShowWindow(FALSE);
}


NAMESPACE_END_GUI
