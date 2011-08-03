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

#include "RadioButton.h"
#include "RadioButtonGroup.h"
#include "HLayout.h"

namespace nux
{

  Color RADIO_TEXT_COLOR = Color (0xFFB0B0B0);

  RadioButton::RadioButton (const TCHAR *Caption, bool state, NUX_FILE_LINE_DECL)
    :   AbstractButton (Caption, NUX_FILE_LINE_PARAM)
  {
    radio_button_group_     = 0;
    m_GroupId   = -1;
    m_hlayout   = 0;
    m_CheckArea = 0;
    m_TextArea  = 0;

    m_hlayout   = new HLayout (NUX_TRACKER_LOCATION);
    m_CheckArea = new InputArea();
    m_TextArea  = new InputArea();

    // Set Signals
    m_CheckArea->mouse_click.connect (sigc::mem_fun (this, &RadioButton::RecvClick) );
    mouse_click.connect (sigc::mem_fun (this, &RadioButton::RecvClick) );

    m_CheckArea->mouse_move.connect (sigc::mem_fun (this, &RadioButton::RecvMouseMove) );
    mouse_move.connect (sigc::mem_fun (this, &RadioButton::RecvMouseMove) );

    m_CheckArea->mouse_enter.connect (sigc::mem_fun (this, &RadioButton::RecvMouseEnter) );
    mouse_enter.connect (sigc::mem_fun (this, &RadioButton::RecvMouseEnter) );

    m_CheckArea->mouse_leave.connect (sigc::mem_fun (this, &RadioButton::RecvMouseLeave) );
    mouse_leave.connect (sigc::mem_fun (this, &RadioButton::RecvMouseLeave) );

    m_CheckArea->mouse_up.connect (sigc::mem_fun (this, &RadioButton::RecvMouseUp) );
    mouse_up.connect (sigc::mem_fun (this, &RadioButton::RecvMouseUp) );

    m_CheckArea->mouse_down.connect (sigc::mem_fun (this, &RadioButton::RecvMouseDown) );
    mouse_down.connect (sigc::mem_fun (this, &RadioButton::RecvMouseDown) );

    // Set Geometry
    m_CheckArea->SetMinimumSize (14, 14);
    m_CheckArea->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT) );

    m_TextArea->SetMinimumSize (14, 14);

    // Do not configure m_TextArea-> This is done in setCaption according to the size of the caption text.

    m_hlayout->SetHorizontalInternalMargin (4);
    m_hlayout->AddView (m_CheckArea, 0);
    m_hlayout->AddView (m_TextArea, 0);

    // This is convenient to make the layout and the RadioButton fit the check area and/or the caption area.
    // Since the check area is bigger than 4x4, it will force the layout and the RadioButton to grow.
    // This is useful if the RadioButton is put in a vertical layout and it has a stretch factor of 0. Then the width of the RadioButton
    // will be adjusted to fit the minimum width of the check area and the caption area.
    {
      m_hlayout->SetMinimumSize (1, 1);
      SetMinimumSize (14, 14);
      ApplyMinWidth();
      ApplyMinHeight();
    }

    SetLayout(m_hlayout);

    SetState (state);
    SetCaption (Caption);

    if (Caption) m_TextArea->mouse_click.connect (sigc::mem_fun (this, &RadioButton::RecvClick) );

    if (Caption) m_TextArea->mouse_move.connect (sigc::mem_fun (this, &RadioButton::RecvMouseMove) );

    if (Caption) m_TextArea->mouse_enter.connect (sigc::mem_fun (this, &RadioButton::RecvMouseEnter) );

    if (Caption) m_TextArea->mouse_leave.connect (sigc::mem_fun (this, &RadioButton::RecvMouseLeave) );

    if (Caption) m_TextArea->mouse_up.connect (sigc::mem_fun (this, &RadioButton::RecvMouseUp) );

    if (Caption) m_TextArea->mouse_down.connect (sigc::mem_fun (this, &RadioButton::RecvMouseDown) );
  }

  RadioButton::~RadioButton()
  {
    if (radio_button_group_ && m_GroupId != -1)
    {
      radio_button_group_->DisconnectButton(this);
      radio_button_group_->UnReference();
      radio_button_group_ = 0;
    }
  }

  long RadioButton::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    ret = m_CheckArea->OnEvent (ievent, ret, ProcessEventInfo);

    if (m_TextArea->GetBaseString().Length() != 0 )
    {
      ret = m_TextArea->OnEvent (ievent, ret, ProcessEventInfo);
    }

    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }

  Area* RadioButton::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    if ((event_type == NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
      return NULL;
    return this;
  }

  void RadioButton::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground (GfxContext, base);
    GetPainter().PaintTextLineStatic (GfxContext, GetFont (), m_TextArea->GetGeometry(), m_TextArea->GetBaseString().GetTCharPtr(), GetTextColor(), eAlignTextLeft);
    InteractState is;
    is.is_on = _state;
    is.is_focus = IsMouseOwner();

    is.is_prelight = IsMouseInside();

    GetPainter().PaintRadioButton (GfxContext, m_CheckArea->GetGeometry(), is, Color (0xff000000) );
  }

  void RadioButton::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void RadioButton::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void RadioButton::SetCaption (const TCHAR *caption)
  {
    if (caption == 0 || (StringLength (caption) == 0) )
    {
      m_TextArea->SetBaseString (TEXT ("") );
    }
    else
      m_TextArea->SetBaseString (caption);

    m_TextArea->SetMinimumWidth (4 + GetFont ()->GetStringWidth (m_TextArea->GetBaseString().GetTCharPtr() ) );
  }

  const NString &RadioButton::GetCaption() const
  {
    return m_TextArea->GetBaseString();
  }

  void RadioButton::SetState (bool State)
  {
    SetStatePrivate (State);
  }

  void RadioButton::SetState (bool State, bool EmitSignal)
  {
    if (radio_button_group_ && State)
    {
      radio_button_group_->SetActiveButton (this, EmitSignal);
      return;
    }
    else if (radio_button_group_ && !State)
    {
      nuxDebugMsg (TEXT ("[RadioButton::SetState] this radioButton is controlled by a RadioButtonGroup. You can't set its state to false directly.") );
      return;
    }

    SetStatePrivate (State, EmitSignal);
  }

  void RadioButton::SetRadioGroupSelector (RadioButtonGroup *RadioSelector)
  {
    if (radio_button_group_ == RadioSelector)
      return;

    if(radio_button_group_)
    {
      radio_button_group_->UnReference();
      radio_button_group_ = 0;
    }

    if(RadioSelector)
    {
      radio_button_group_ = RadioSelector;
      radio_button_group_->Reference();
    }
  }

  RadioButtonGroup* RadioButton::GetRadioGroupSelector()
  {
    return radio_button_group_;
  }

  void RadioButton::SetStatePrivate (bool State)
  {
    _state = State;
    QueueDraw();
  }

  void RadioButton::SetStatePrivate (bool State, bool EmitSignal)
  {
    _state = State;

    if (EmitSignal)
    {
      sigToggled.emit();
      sigStateChanged.emit (_state);
    }

    QueueDraw();
  }

  bool RadioButton::GetState() const
  {
    return _state;
  }

  void RadioButton::RecvClick (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (radio_button_group_)
    {
      radio_button_group_->NotifyClick (this);
    }
    else
    {
      _state = !_state;
      sigToggled.emit();
      sigStateToggled.emit (this);
      sigStateChanged.emit (_state);
    }

    QueueDraw();
  }

  void RadioButton::RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    QueueDraw();
  }

  void RadioButton::RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    QueueDraw();
  }

  void RadioButton::RecvMouseMove (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    QueueDraw();
  }

  void RadioButton::RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    QueueDraw();
  }

  void RadioButton::RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    QueueDraw();
  }

  void RadioButton::EmitStateChangedSignal()
  {
    sigStateChanged.emit (_state);
  }


}
