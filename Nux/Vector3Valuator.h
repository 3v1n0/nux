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


#ifndef VECTOR3VALUATOR_H
#define VECTOR3VALUATOR_H

namespace nux   //NUX_NAMESPACE_BEGIN
{

  class EditTextBox;

  class Vector3Valuator: public View
  {
  public:
    Vector3Valuator (NUX_FILE_LINE_PROTO);
    ~Vector3Valuator();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo) = 0;
    virtual void Draw (GraphicsContext &GfxContext, bool force_draw) = 0;
    virtual void DrawContent (GraphicsContext &GfxContext, bool force_draw) = 0;
    virtual void PostDraw (GraphicsContext &GfxContext, bool force_draw) = 0;

    //  RECEIVERS
    void RecvMouseMove (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvDecrement (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void TimerSpinUpBtn (void *v);
    void TimerSpinDownBtn (void *v);
    void RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags);
    /*
        The spinner buttons need to be redrawn when they are released.
        Their color may have changed when the mouse down was initiated.
        This receiver is going to initiate the redraw of the whole Vector3Valuator.
    */
    void RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvStartKeyboardFocus (EditTextBox *textbox);
    void RecvEndKeyboardFocus (EditTextBox *textbox);
    void RecvEscapeKeyboardFocus (EditTextBox *textbox);
    void RecvEditChange (EditTextBox *textbox);
    void RecvValidateEntryX (EditTextBox *textbox);
    void RecvValidateEntryY (EditTextBox *textbox);
    void RecvValidateEntryZ (EditTextBox *textbox);

    virtual void ImplementIncrementX() = 0;
    virtual void ImplementDecrementX() = 0;
    virtual void ImplementIncrementY() = 0;
    virtual void ImplementDecrementY() = 0;
    virtual void ImplementIncrementZ() = 0;
    virtual void ImplementDecrementZ() = 0;
    virtual void ImplementValidateEntryX() = 0;
    virtual void ImplementValidateEntryY() = 0;
    virtual void ImplementValidateEntryZ() = 0;


    //  SIGNALS
    sigc::signal<void> sigClick;
    sigc::signal<void> sigIncrement;
    sigc::signal<void> sigDecrement;
    sigc::signal<void> sigValueChanged;
    sigc::signal<void, int> sigIntValueChanged;

  protected:
    EditTextBox    *m_XEdit;
    EditTextBox    *m_YEdit;
    EditTextBox    *m_ZEdit;
    CoreArea       *m_MouseControlledButton;
    int m_Mouse;

  };

} //NUX_NAMESPACE_END

#endif // VECTOR3VALUATOR_H

