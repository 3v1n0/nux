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


#ifndef SPINBOX_LOGIC_H
#define SPINBOX_LOGIC_H

#include "EditTextBox.h"
#include "View.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

  class SpinBox_Logic: public View
  {
  public:
    SpinBox_Logic (NUX_FILE_LINE_PROTO);
    virtual ~SpinBox_Logic();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo) = 0;
    virtual void Draw (GraphicsContext &GfxContext, bool force_draw) = 0;
    virtual void DrawContent (GraphicsContext &GfxContext, bool force_draw) = 0;
    virtual void PostDraw (GraphicsContext &GfxContext, bool force_draw) = 0;

    //  RECEIVERS
    void RecvMouseMove (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvIncrement (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvDecrement (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void TimerSpinUpBtn (void *v);
    void TimerSpinDownBtn (void *v);
    void RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags);
    /*
        The spinner buttons need to be redrawn when they are released.
        Their color may have changed when the mouse down was initiated.
        This receiver is going to initiate the redraw of the whole SpinBox_Logic.
    */
    void RecvSpinnerMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvStartKeyboardFocus (EditTextBox *textbox);
    void RecvEndKeyboardFocus (EditTextBox *textbox);
    void RecvEscapeKeyboardFocus (EditTextBox *textbox);
    void RecvEditChange (EditTextBox *textbox);
    void RecvValidateEntry (EditTextBox *textbox);

    virtual void ImplementIncrementBtn() = 0;
    virtual void ImplementDecrementBtn() = 0;
    virtual void ImplementValidateEntry() = 0;


    //  SIGNALS
    sigc::signal<void> sigClick;
    sigc::signal<void> sigIncrement;
    sigc::signal<void> sigDecrement;
    sigc::signal<void> sigValueChanged;
    sigc::signal<void, int> sigIntValueChanged;

  protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

  protected:
    EditTextBox    *m_EditLine;
    CoreArea       *m_SpinnerUpBtn;
    CoreArea       *m_SpinnerDownBtn;

    TimerFunctor *m_UpTimerCallback;
    TimerFunctor *m_DownTimerCallback;
    TimerHandle m_UpTimerHandler;
    TimerHandle m_DownTimerHandler;
  };

} //NUX_NAMESPACE_END

#endif // SPINBOX_LOGIC_H
