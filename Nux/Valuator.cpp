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
#include "Valuator.h"

namespace nux { //NUX_NAMESPACE_BEGIN

Valuator::Valuator(NUX_FILE_LINE_DECL)
:   ActiveInterfaceObject(NUX_FILE_LINE_PARAM)
,   m_Mouse(0)
{
    m_MouseControlledButton = smptr(CoreArea)(new CoreArea());
    m_EditLine = smptr(EditTextBox)(new EditTextBox(TEXT(""), NUX_TRACKER_LOCATION));
    // Set Original State
    m_EditLine->SetSuffix(TEXT(""));
    m_EditLine->SetPrefix(TEXT(""));

    // Set Signals
    m_MouseControlledButton->OnMouseDown.connect(sigc::mem_fun(this, &Valuator::RecvMouseDown));
    m_MouseControlledButton->OnMouseDoubleClick.connect(sigc::mem_fun(this, &Valuator::RecvMouseDown));
    m_MouseControlledButton->OnMouseUp.connect(sigc::mem_fun(this, &Valuator::RecvMouseUp));
    m_MouseControlledButton->OnMouseDrag.connect(sigc::mem_fun(this, &Valuator::RecvMouseDrag));
    m_MouseControlledButton->OnMouseEnter.connect(sigc::mem_fun(this, &Valuator::RecvMouseEnter));
    m_MouseControlledButton->OnMouseLeave.connect(sigc::mem_fun(this, &Valuator::RecvMouseLeave));

    m_EditLine->sigValidateEntry.connect(sigc::mem_fun(this, &Valuator::RecvValidateEntry));
    m_EditLine->sigStartKeyboardFocus.connect(sigc::mem_fun(this, &Valuator::RecvStartKeyboardFocus));
    m_EditLine->sigEndKeyboardFocus.connect(sigc::mem_fun(this, &Valuator::RecvEndKeyboardFocus));
    m_EditLine->sigEscapeKeyboardFocus.connect(sigc::mem_fun(this, &Valuator::RecvEscapeKeyboardFocus));
    m_EditLine->sigEditChange.connect(sigc::mem_fun(this, &Valuator::RecvEditChange));
    m_EditLine->OnMouseEnter.connect(sigc::mem_fun(this, &Valuator::RecvMouseEnter));
    m_EditLine->OnMouseLeave.connect(sigc::mem_fun(this, &Valuator::RecvMouseLeave));

}

Valuator::~Valuator()
{
}

void Valuator::RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{ 
    if(button_flags & NUX_EVENT_BUTTON1)
        m_Mouse = 1;
    else if(button_flags & NUX_EVENT_BUTTON3)
        m_Mouse = 3;
    NeedRedraw();
}

void Valuator::RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    if((m_Mouse == 1) && (button_flags & NUX_EVENT_BUTTON1) )
        m_Mouse = 0;
    else if((m_Mouse == 3) && (button_flags & NUX_EVENT_BUTTON3) )
        m_Mouse = 0;
    NeedRedraw();
}

void Valuator::RecvDecrement(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    TimerSpinDownBtn(0);
}

void Valuator::TimerSpinUpBtn(void* v)
{
    ImplementIncrementBtn();
}

void Valuator::TimerSpinDownBtn(void* v)
{
    ImplementDecrementBtn();
}

void Valuator::RecvStartKeyboardFocus(const weaksmptr(EditTextBox) textbox)
{
    NeedRedraw();
}

void Valuator::RecvEndKeyboardFocus(const weaksmptr(EditTextBox) textbox)
{
    NeedRedraw();
}

void Valuator::RecvEscapeKeyboardFocus(const weaksmptr(EditTextBox) textbox)
{
    NeedRedraw();
}

void Valuator::RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void Valuator::RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void Valuator::RecvMouseMove(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void Valuator::RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
    if(m_Mouse == 1)
    {
        if(dx > 0)
            TimerSpinUpBtn(0);
        if(dx < 0)
            TimerSpinDownBtn(0);
    }

    if(m_Mouse == 3)
    {
        if(dy > 0)
            TimerSpinDownBtn(0);
        if(dy < 0)
            TimerSpinUpBtn(0);
    }

    NeedRedraw();
}

void Valuator::RecvEditChange(const weaksmptr(EditTextBox) textbox)
{
    NeedRedraw();
}

void Valuator::RecvValidateEntry(const weaksmptr(EditTextBox) textbox)
{
    ImplementValidateEntry();

//    int ret = 0;
//    if(inlCharToInteger(m_EditLine->GetCleanCaption().GetTChar(), ret))
//    {
//        m_iValue = ret;
//        if(m_iValue < m_IntValidator.GetMinimum())
//        {
//            m_iValue = m_IntValidator.GetMinimum();
//            m_EditLine->setCaption(inlPrintf("%d", m_iValue));
//        }
//        if(m_iValue > m_IntValidator.GetMaximum())
//        {
//            m_iValue = m_IntValidator.GetMaximum();
//            m_EditLine->setCaption(inlPrintf("%d", m_iValue));
//        }
//    }
//    else
//    {
//        m_EditLine->setCaption(inlPrintf("%d", m_iValue));
//    }
}


} //NUX_NAMESPACE_END
