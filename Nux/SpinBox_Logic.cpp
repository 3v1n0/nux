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
#include "SpinBox_Logic.h"

namespace nux
{

  SpinBox_Logic::SpinBox_Logic (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_UpTimerHandler (0)
    ,   m_DownTimerHandler (0)
  {
    InitializeLayout();
    InitializeWidgets();
  }

  SpinBox_Logic::~SpinBox_Logic()
  {
    DestroyLayout();
  }

  void SpinBox_Logic::InitializeWidgets()
  {
    // Set Original State
    m_EditLine->SetSuffix (TEXT ("") );
    m_EditLine->SetPrefix (TEXT ("") );

    // Set Signals
    m_SpinnerUpBtn->OnMouseDown.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvIncrement) );
    m_SpinnerUpBtn->OnMouseDoubleClick.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvIncrement) );
    m_SpinnerUpBtn->OnMouseUp.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvSpinnerMouseUp) );
    m_SpinnerUpBtn->OnMouseClick.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvSpinnerMouseUp) );
    m_SpinnerUpBtn->OnMouseEnter.connect (sigc::mem_fun (this, &SpinBox_Logic::RecvMouseEnter) );
    m_SpinnerUpBtn->OnMouseLeave.connect (sigc::mem_fun (this, &SpinBox_Logic::RecvMouseLeave) );

    m_SpinnerDownBtn->OnMouseDown.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvDecrement) );
    m_SpinnerDownBtn->OnMouseDoubleClick.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvDecrement) );
    m_SpinnerDownBtn->OnMouseUp.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvSpinnerMouseUp) );
    m_SpinnerDownBtn->OnMouseClick.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvSpinnerMouseUp) );
    m_SpinnerDownBtn->OnMouseEnter.connect (sigc::mem_fun (this, &SpinBox_Logic::RecvMouseEnter) );
    m_SpinnerDownBtn->OnMouseLeave.connect (sigc::mem_fun (this, &SpinBox_Logic::RecvMouseLeave) );

    m_EditLine->sigValidateEntry.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvValidateEntry) );
    m_EditLine->sigStartKeyboardFocus.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvStartKeyboardFocus) );
    m_EditLine->sigEndKeyboardFocus.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvEndKeyboardFocus) );
    m_EditLine->sigEscapeKeyboardFocus.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvEscapeKeyboardFocus) );
    m_EditLine->sigEditChange.connect ( sigc::mem_fun (this, &SpinBox_Logic::RecvEditChange) );
    m_EditLine->OnMouseEnter.connect (sigc::mem_fun (this, &SpinBox_Logic::RecvMouseEnter) );
    m_EditLine->OnMouseLeave.connect (sigc::mem_fun (this, &SpinBox_Logic::RecvMouseLeave) );

    m_UpTimerCallback = new TimerFunctor;
    m_UpTimerCallback->OnTimerExpired.connect (sigc::mem_fun (this, &SpinBox_Logic::TimerSpinUpBtn) );
    m_DownTimerCallback = new TimerFunctor;
    m_DownTimerCallback->OnTimerExpired.connect (sigc::mem_fun (this, &SpinBox_Logic::TimerSpinDownBtn) );
  }

  void SpinBox_Logic::InitializeLayout()
  {
    m_SpinnerUpBtn      = new CoreArea (NUX_TRACKER_LOCATION);
    m_SpinnerDownBtn    = new CoreArea (NUX_TRACKER_LOCATION);
    m_EditLine          = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);
  }

  void SpinBox_Logic::DestroyLayout()
  {
  }

  void SpinBox_Logic::RecvIncrement (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    TimerSpinUpBtn (0);
  }

  void SpinBox_Logic::RecvSpinnerMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_UpTimerHandler.IsValid() )
    {
      GetTimer().RemoveTimerHandler (m_UpTimerHandler);
      m_UpTimerHandler = 0;
    }

    if (m_DownTimerHandler.IsValid() )
    {
      GetTimer().RemoveTimerHandler (m_DownTimerHandler);
      m_DownTimerHandler = 0;
    }

    NeedRedraw();
  }

  void SpinBox_Logic::RecvDecrement (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    TimerSpinDownBtn (0);
  }

  void SpinBox_Logic::TimerSpinUpBtn (void *v)
  {
    ImplementIncrementBtn();
  }

  void SpinBox_Logic::TimerSpinDownBtn (void *v)
  {
    ImplementDecrementBtn();
  }

  void SpinBox_Logic::RecvStartKeyboardFocus (EditTextBox *textbox)
  {
    NeedRedraw();
  }

  void SpinBox_Logic::RecvEndKeyboardFocus (EditTextBox *textbox)
  {
    NeedRedraw();
  }

  void SpinBox_Logic::RecvEscapeKeyboardFocus (EditTextBox *textbox)
  {
    NeedRedraw();
  }

  void SpinBox_Logic::RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void SpinBox_Logic::RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void SpinBox_Logic::RecvMouseMove (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void SpinBox_Logic::RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void SpinBox_Logic::RecvEditChange (EditTextBox *textbox)
  {
    NeedRedraw();
  }

  void SpinBox_Logic::RecvValidateEntry (EditTextBox *textbox)
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

}
