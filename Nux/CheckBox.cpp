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
#include "EditTextBox.h"
#include "CheckBox.h"

namespace nux
{

  CheckBox::CheckBox (const TCHAR *Caption, bool state, NUX_FILE_LINE_DECL)
    :   AbstractButton (Caption, NUX_FILE_LINE_PARAM)
  {
    m_hlayout = 0;
    m_TextArea = 0;

    m_hlayout   = new HLayout (NUX_TRACKER_LOCATION);
    m_CheckArea = new InputArea(NUX_TRACKER_LOCATION);
    m_TextArea  = new InputArea(NUX_TRACKER_LOCATION);

    // Set Signals
    m_CheckArea->OnMouseClick.connect (sigc::mem_fun (this, &CheckBox::RecvClick) );
    OnMouseClick.connect (sigc::mem_fun (this, &CheckBox::RecvClick) );

    m_CheckArea->OnMouseMove.connect (sigc::mem_fun (this, &CheckBox::RecvMouseMove) );
    OnMouseMove.connect (sigc::mem_fun (this, &CheckBox::RecvMouseMove) );

    m_CheckArea->OnMouseEnter.connect (sigc::mem_fun (this, &CheckBox::RecvMouseEnter) );
    OnMouseEnter.connect (sigc::mem_fun (this, &CheckBox::RecvMouseEnter) );

    m_CheckArea->OnMouseLeave.connect (sigc::mem_fun (this, &CheckBox::RecvMouseLeave) );
    OnMouseLeave.connect (sigc::mem_fun (this, &CheckBox::RecvMouseLeave) );

    m_CheckArea->OnMouseUp.connect (sigc::mem_fun (this, &CheckBox::RecvMouseUp) );
    OnMouseUp.connect (sigc::mem_fun (this, &CheckBox::RecvMouseUp) );

    m_CheckArea->OnMouseDown.connect (sigc::mem_fun (this, &CheckBox::RecvMouseDown) );
    OnMouseDown.connect (sigc::mem_fun (this, &CheckBox::RecvMouseDown) );

    // Set Geometry
    m_CheckArea->SetMinimumSize (14, 14);
    m_CheckArea->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT) );

    m_TextArea->SetMinimumSize (14, 14);

    // Do not configure m_TextArea-> This is done in setCaption according to the size of the caption text.

    m_hlayout->SetHorizontalInternalMargin (4);
    m_hlayout->AddView (m_CheckArea, 0);
    m_hlayout->AddView (m_TextArea, 0);

    // This is convenient to make the layout and the CheckBox fit the check area and the caption area.
    // Since the check area is bigger than 4x4, it will force the layout and the CheckBox to grow.
    // This is useful if the CheckBox is put in a vertical layout and it has a stretch factor of 0. Then the width of the CheckBox
    // will be adjusted to fit the minimum width of the check area and the caption area.
    {
      m_hlayout->SetMinimumSize (1, 1);
      SetMinimumSize (14, 14);
      ApplyMinWidth();
      ApplyMinHeight();
    }

    SetLayout (m_hlayout);

    SetState (state);
    SetCaption (Caption);

    if (Caption) m_TextArea->OnMouseClick.connect (sigc::mem_fun (this, &CheckBox::RecvClick) );

    if (Caption) m_TextArea->OnMouseMove.connect (sigc::mem_fun (this, &CheckBox::RecvMouseMove) );

    if (Caption) m_TextArea->OnMouseEnter.connect (sigc::mem_fun (this, &CheckBox::RecvMouseEnter) );

    if (Caption) m_TextArea->OnMouseLeave.connect (sigc::mem_fun (this, &CheckBox::RecvMouseLeave) );

    if (Caption) m_TextArea->OnMouseUp.connect (sigc::mem_fun (this, &CheckBox::RecvMouseUp) );

    if (Caption) m_TextArea->OnMouseDown.connect (sigc::mem_fun (this, &CheckBox::RecvMouseDown) );
  }

  CheckBox::~CheckBox()
  {
  }

  long CheckBox::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
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

  Area* CheckBox::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    if ((event_type == NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
      return NULL;
    return this;
  }

  void CheckBox::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground (GfxContext, base);
    GetPainter().PaintTextLineStatic (GfxContext, GetFont (), m_TextArea->GetGeometry(), m_TextArea->GetBaseString().GetTCharPtr(), GetTextColor(), eAlignTextLeft);
    InteractState is;
    is.is_on = _state;
    is.is_focus = IsMouseOwner();

    is.is_prelight = IsMouseInside();

    GetPainter().PaintCheckBox (GfxContext, m_CheckArea->GetGeometry(), is, Color (0xff000000) );
  }

  void CheckBox::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void CheckBox::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void CheckBox::SetCaption (const TCHAR *caption)
  {
    if (caption == 0 || (StringLength (caption) == 0) )
    {
      m_TextArea->SetBaseString (TEXT ("") );
    }
    else
      m_TextArea->SetBaseString (caption);

    m_TextArea->SetMinimumWidth (4 + GetFont ()->GetStringWidth (m_TextArea->GetBaseString().GetTCharPtr() ) );
  }

  const NString &CheckBox::GetCaption() const
  {
    return m_TextArea->GetBaseString();
  }


  void CheckBox::SetState (bool State)
  {
    _state = State;
    NeedRedraw();
  }

  void CheckBox::SetState (bool State, bool EmitSignal)
  {
    _state = State;

    if (EmitSignal)
    {
      sigToggled.emit();
      sigStateChanged.emit (_state);
    }

    NeedRedraw();
  }

  bool CheckBox::GetState() const
  {
    return _state;
  }

  void CheckBox::RecvClick (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    _state = !_state;
    sigStateToggled.emit (this);
    sigStateChanged.emit (_state);
    NeedRedraw();
  }

  void CheckBox::RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void CheckBox::RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void CheckBox::RecvMouseMove (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void CheckBox::RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void CheckBox::RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void CheckBox::EmitStateSignal()
  {
    sigStateChanged.emit (_state);
  }


}
