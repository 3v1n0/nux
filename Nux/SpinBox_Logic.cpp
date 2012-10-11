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
#include "TimerProc.h"
#include "SpinBox_Logic.h"

namespace nux
{

  SpinBox_Logic::SpinBox_Logic(NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
    ,   m_UpTimerHandler(0)
    ,   m_DownTimerHandler(0)
  {
    m_SpinnerUpBtn      = new BasicView(NUX_TRACKER_LOCATION);
    m_SpinnerDownBtn    = new BasicView(NUX_TRACKER_LOCATION);
    m_EditLine          = new EditTextBox("", NUX_TRACKER_LOCATION);

    // Set Original State
    m_EditLine->SetSuffix("");
    m_EditLine->SetPrefix("");

    // Set Signals
    m_SpinnerUpBtn->mouse_down.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvIncrement));
    m_SpinnerUpBtn->mouse_double_click.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvIncrement));
    m_SpinnerUpBtn->mouse_up.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvSpinnerMouseUp));
    m_SpinnerUpBtn->mouse_click.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvSpinnerMouseUp));
    m_SpinnerUpBtn->mouse_enter.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvMouseEnter));
    m_SpinnerUpBtn->mouse_leave.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvMouseLeave));

    m_SpinnerDownBtn->mouse_down.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvDecrement));
    m_SpinnerDownBtn->mouse_double_click.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvDecrement));
    m_SpinnerDownBtn->mouse_up.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvSpinnerMouseUp));
    m_SpinnerDownBtn->mouse_click.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvSpinnerMouseUp));
    m_SpinnerDownBtn->mouse_enter.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvMouseEnter));
    m_SpinnerDownBtn->mouse_leave.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvMouseLeave));

    m_EditLine->sigValidateEntry.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvValidateEntry));
    m_EditLine->sigStartKeyboardFocus.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvStartKeyboardFocus));
    m_EditLine->sigEndKeyboardFocus.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvEndKeyboardFocus));
    m_EditLine->sigEscapeKeyboardFocus.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvEscapeKeyboardFocus));
    m_EditLine->sigEditChange.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvEditChange));
    m_EditLine->mouse_enter.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvMouseEnter));
    m_EditLine->mouse_leave.connect(sigc::mem_fun(this, &SpinBox_Logic::RecvMouseLeave));

    m_UpTimerCallback = new TimerFunctor;
    m_UpTimerCallback->tick.connect(sigc::mem_fun(this, &SpinBox_Logic::TimerSpinUpBtn));
    m_DownTimerCallback = new TimerFunctor;
    m_DownTimerCallback->tick.connect(sigc::mem_fun(this, &SpinBox_Logic::TimerSpinDownBtn));
  }

  SpinBox_Logic::~SpinBox_Logic()
  {
    delete m_UpTimerCallback;
    delete m_DownTimerCallback;
//     m_SpinnerUpBtn->Dispose();
//     m_SpinnerDownBtn->Dispose();
//     m_EditLine->Dispose();
  }

  void SpinBox_Logic::RecvIncrement(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    TimerSpinUpBtn(0);
  }

  void SpinBox_Logic::RecvSpinnerMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (m_UpTimerHandler.IsValid())
    {
      GetTimer().RemoveTimerHandler(m_UpTimerHandler);
      m_UpTimerHandler = 0;
    }

    if (m_DownTimerHandler.IsValid())
    {
      GetTimer().RemoveTimerHandler(m_DownTimerHandler);
      m_DownTimerHandler = 0;
    }

    QueueDraw();
  }

  void SpinBox_Logic::RecvDecrement(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    TimerSpinDownBtn(0);
  }

  void SpinBox_Logic::TimerSpinUpBtn(void * /* v */)
  {
    ImplementIncrementBtn();
  }

  void SpinBox_Logic::TimerSpinDownBtn(void * /* v */)
  {
    ImplementDecrementBtn();
  }

  void SpinBox_Logic::RecvStartKeyboardFocus(EditTextBox * /* textbox */)
  {
    QueueDraw();
  }

  void SpinBox_Logic::RecvEndKeyboardFocus(EditTextBox * /* textbox */)
  {
    QueueDraw();
  }

  void SpinBox_Logic::RecvEscapeKeyboardFocus(EditTextBox * /* textbox */)
  {
    QueueDraw();
  }

  void SpinBox_Logic::RecvMouseEnter(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void SpinBox_Logic::RecvMouseLeave(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void SpinBox_Logic::RecvMouseMove(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void SpinBox_Logic::RecvMouseDrag(int /* x */, int /* y */, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void SpinBox_Logic::RecvEditChange(EditTextBox * /* textbox */)
  {
    QueueDraw();
  }

  void SpinBox_Logic::RecvValidateEntry(EditTextBox * /* textbox */)
  {
    ImplementValidateEntry();

//    int ret = 0;
//    if (inlCharToInteger(m_EditLine->GetCleanCaption().GetTChar(), ret))
//    {
//        m_iValue = ret;
//        if (m_iValue < m_IntValidator.GetMinimum())
//        {
//            m_iValue = m_IntValidator.GetMinimum();
//            m_EditLine->setCaption(std::string::Printf("%d", m_iValue));
//        }
//        if (m_iValue > m_IntValidator.GetMaximum())
//        {
//            m_iValue = m_IntValidator.GetMaximum();
//            m_EditLine->setCaption(std::string::Printf("%d", m_iValue));
//        }
//    }
//    else
//    {
//        m_EditLine->setCaption(std::string::Printf("%d", m_iValue));
//    }
  }

}
