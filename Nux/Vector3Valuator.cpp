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
#include "TimerProc.h"
#include "EditTextBox.h"
#include "Vector3Valuator.h"

NAMESPACE_BEGIN_GUI

Vector3Valuator::Vector3Valuator(NUX_FILE_LINE_DECL)
:   ActiveInterfaceObject(NUX_FILE_LINE_PARAM)
,   m_Mouse(0)
{
    m_XEdit = smptr(EditTextBox)(new EditTextBox());
    m_YEdit = smptr(EditTextBox)(new EditTextBox());
    m_ZEdit = smptr(EditTextBox)(new EditTextBox());
    m_MouseControlledButton = smptr(CoreArea)(new CoreArea());
    // Set Original State
    m_XEdit->SetSuffix(TEXT(""));
    m_XEdit->SetPrefix(TEXT(""));
    m_YEdit->SetSuffix(TEXT(""));
    m_YEdit->SetPrefix(TEXT(""));
    m_ZEdit->SetSuffix(TEXT(""));
    m_ZEdit->SetPrefix(TEXT(""));

    // Set Signals
    m_MouseControlledButton->OnMouseDown.connect( sigc::mem_fun(this, &Vector3Valuator::RecvMouseDown));
    m_MouseControlledButton->OnMouseDoubleClick.connect( sigc::mem_fun(this, &Vector3Valuator::RecvMouseDown));
    m_MouseControlledButton->OnMouseUp.connect( sigc::mem_fun(this, &Vector3Valuator::RecvMouseUp));
    m_MouseControlledButton->OnMouseDrag.connect( sigc::mem_fun(this, &Vector3Valuator::RecvMouseDrag));
    m_MouseControlledButton->OnMouseEnter.connect(sigc::mem_fun(this, &Vector3Valuator::RecvMouseEnter));
    m_MouseControlledButton->OnMouseLeave.connect(sigc::mem_fun(this, &Vector3Valuator::RecvMouseLeave));

    m_XEdit->sigValidateEntry.connect( sigc::mem_fun(this, &Vector3Valuator::RecvValidateEntryX));
    m_XEdit->sigStartKeyboardFocus.connect( sigc::mem_fun(this, &Vector3Valuator::RecvStartKeyboardFocus));
    m_XEdit->sigEndKeyboardFocus.connect( sigc::mem_fun(this, &Vector3Valuator::RecvEndKeyboardFocus));
    m_XEdit->sigEscapeKeyboardFocus.connect( sigc::mem_fun(this, &Vector3Valuator::RecvEscapeKeyboardFocus));
    m_XEdit->sigEditChange.connect( sigc::mem_fun(this, &Vector3Valuator::RecvEditChange));
    m_XEdit->OnMouseEnter.connect(sigc::mem_fun(this, &Vector3Valuator::RecvMouseEnter));
    m_XEdit->OnMouseLeave.connect(sigc::mem_fun(this, &Vector3Valuator::RecvMouseLeave));

    m_YEdit->sigValidateEntry.connect( sigc::mem_fun(this, &Vector3Valuator::RecvValidateEntryY));
    m_YEdit->sigStartKeyboardFocus.connect( sigc::mem_fun(this, &Vector3Valuator::RecvStartKeyboardFocus));
    m_YEdit->sigEndKeyboardFocus.connect( sigc::mem_fun(this, &Vector3Valuator::RecvEndKeyboardFocus));
    m_YEdit->sigEscapeKeyboardFocus.connect( sigc::mem_fun(this, &Vector3Valuator::RecvEscapeKeyboardFocus));
    m_YEdit->sigEditChange.connect( sigc::mem_fun(this, &Vector3Valuator::RecvEditChange));
    m_YEdit->OnMouseEnter.connect(sigc::mem_fun(this, &Vector3Valuator::RecvMouseEnter));
    m_YEdit->OnMouseLeave.connect(sigc::mem_fun(this, &Vector3Valuator::RecvMouseLeave));

    m_ZEdit->sigValidateEntry.connect( sigc::mem_fun(this, &Vector3Valuator::RecvValidateEntryZ));
    m_ZEdit->sigStartKeyboardFocus.connect( sigc::mem_fun(this, &Vector3Valuator::RecvStartKeyboardFocus));
    m_ZEdit->sigEndKeyboardFocus.connect( sigc::mem_fun(this, &Vector3Valuator::RecvEndKeyboardFocus));
    m_ZEdit->sigEscapeKeyboardFocus.connect( sigc::mem_fun(this, &Vector3Valuator::RecvEscapeKeyboardFocus));
    m_ZEdit->sigEditChange.connect( sigc::mem_fun(this, &Vector3Valuator::RecvEditChange));
    m_ZEdit->OnMouseEnter.connect(sigc::mem_fun(this, &Vector3Valuator::RecvMouseEnter));
    m_ZEdit->OnMouseLeave.connect(sigc::mem_fun(this, &Vector3Valuator::RecvMouseLeave));

}

Vector3Valuator::~Vector3Valuator()
{
}

void Vector3Valuator::RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{ 
    if( (button_flags & NUX_STATE_BUTTON1_DOWN) && (button_flags & NUX_STATE_BUTTON3_DOWN) )
        m_Mouse = 2;
    else if(button_flags & NUX_EVENT_BUTTON1)
        m_Mouse = 1;
    else if(button_flags & NUX_EVENT_BUTTON3)
        m_Mouse = 3;
    NeedRedraw();
}

void Vector3Valuator::RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    if((m_Mouse == 1) && (button_flags & NUX_EVENT_BUTTON1) )
        m_Mouse = 0;
    else if((m_Mouse == 3) && (button_flags & NUX_EVENT_BUTTON3) )
        m_Mouse = 0;
    NeedRedraw();
}

void Vector3Valuator::RecvDecrement(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    TimerSpinDownBtn(0);
}

void Vector3Valuator::TimerSpinUpBtn(void* v)
{
}

void Vector3Valuator::TimerSpinDownBtn(void* v)
{
}

void Vector3Valuator::RecvStartKeyboardFocus(const weaksmptr(EditTextBox) textbox)
{
    NeedRedraw();
}

void Vector3Valuator::RecvEndKeyboardFocus(const weaksmptr(EditTextBox) textbox)
{
    NeedRedraw();
}

void Vector3Valuator::RecvEscapeKeyboardFocus(const weaksmptr(EditTextBox) textbox)
{
    NeedRedraw();
}

void Vector3Valuator::RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void Vector3Valuator::RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void Vector3Valuator::RecvMouseMove(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void Vector3Valuator::RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
    if(m_Mouse == 2)
    {
        if(dy > 0)
            ImplementDecrementZ();
        if(dy < 0)
            ImplementIncrementZ();
    }
    else if(m_Mouse == 1)
    {
        if(dy > 0)
            ImplementDecrementX();
        if(dy < 0)
            ImplementIncrementX();
    }
    else if(m_Mouse == 3)
    {
        if(dy > 0)
            ImplementDecrementY();
        if(dy < 0)
            ImplementIncrementY();
    }

    NeedRedraw();
}

void Vector3Valuator::RecvEditChange(const weaksmptr(EditTextBox) textbox)
{
    NeedRedraw();
}

void Vector3Valuator::RecvValidateEntryX(const weaksmptr(EditTextBox) textbox)
{
    ImplementValidateEntryX();
}

void Vector3Valuator::RecvValidateEntryY(const weaksmptr(EditTextBox) textbox)
{
    ImplementValidateEntryY();
}

void Vector3Valuator::RecvValidateEntryZ(const weaksmptr(EditTextBox) textbox)
{
    ImplementValidateEntryZ();
}


NAMESPACE_END_GUI
