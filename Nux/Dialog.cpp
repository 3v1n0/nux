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
#include "HLayout.h"
#include "VLayout.h"
#include "Button.h"
#include "Validator.h"
#include "WindowCompositor.h"
#include "Dialog.h"

namespace nux
{

  Dialog::Dialog(const char *WindowName /*  = "" */)
    :   FloatingWindow(WindowName)
  {
    ShowWindow(false);

    m_ButtonLayout = new HLayout("Dialog Buttons");

    m_OkButton = new Button("OK");
    m_OkButton->SetMinimumWidth(60);
    m_OkButton->SetMinimumHeight(20);

    m_CancelButton = new Button("Cancel");
    m_CancelButton->SetMinimumWidth(60);
    m_CancelButton->SetMinimumHeight(20);

    //FIXME - m_OkButton->sigClick.connect(sigc::mem_fun(this, &Dialog::RecvOk));
    //FIXME - m_CancelButton->sigClick.connect(sigc::mem_fun(this, &Dialog::RecvCancel));

    m_ButtonLayout->SetHorizontalInternalMargin(6);
    m_ButtonLayout->SetVerticalExternalMargin(2);
    m_ButtonLayout->AddView(m_OkButton, 0);
    m_ButtonLayout->AddView(m_CancelButton, 0);

    // NuxNote: m_ButtonLayout should be moved to the places where it is used... For instance ColorPickerDialog.
  }


  Dialog::~Dialog()
  {
  }

  void Dialog::Start(bool StartModal)
  {
    ShowWindow(true, StartModal);
  }


}
