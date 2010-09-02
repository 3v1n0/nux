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
#include "HLayout.h"
#include "VLayout.h"
#include "Button.h"
#include "Validator.h"
#include "WindowCompositor.h"
#include "Dialog.h"

NAMESPACE_BEGIN_GUI

Dialog::Dialog(TCHAR* WindowName /*  = TEXT("") */)
:   FloatingWindow(WindowName)
{
    ShowWindow(false);

    m_ButtonLayout = smptr(HLayout)(new HLayout(TEXT("Dialog Buttons")));

    m_OkButton = smptr(Button)(new Button(TEXT("OK")));
    m_OkButton->SetMinimumWidth(60);
    m_OkButton->SetMinimumHeight(20);

    m_CancelButton = smptr(Button)(new Button(TEXT("Cancel")));
    m_CancelButton->SetMinimumWidth(60);
    m_CancelButton->SetMinimumHeight(20);

    m_OkButton->sigClick.connect(sigc::mem_fun(this, &Dialog::RecvOk));
    m_CancelButton->sigClick.connect(sigc::mem_fun(this, &Dialog::RecvCancel));

    m_ButtonLayout->SetHorizontalInternalMargin(6);
    m_ButtonLayout->SetVerticalExternalMargin(2);
    m_ButtonLayout->AddActiveInterfaceObject(m_OkButton, 0);
    m_ButtonLayout->AddActiveInterfaceObject(m_CancelButton, 0);
}


Dialog::~Dialog()
{
}

void Dialog::Start(bool StartModal)
{
    ShowWindow(true, StartModal);
}


NAMESPACE_END_GUI