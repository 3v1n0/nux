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
#include "PushButton.h"
#include "HLayout.h"

namespace nux
{

  PushButton::PushButton (const TCHAR *Caption, NUX_FILE_LINE_DECL)
    :   AbstractButton (Caption, NUX_FILE_LINE_PARAM)
  {
    _state     = false;

    // Set Signals
    OnMouseClick.connect (sigc::mem_fun (this, &PushButton::RecvClick) );
    OnMouseDown.connect (sigc::mem_fun (this, &PushButton::RecvMouseDown) );
    OnMouseDoubleClick.connect (sigc::mem_fun (this, &PushButton::RecvMouseDown) );
    OnMouseUp.connect (sigc::mem_fun (this, &PushButton::RecvMouseUp) );
    OnMouseMove.connect (sigc::mem_fun (this, &PushButton::RecvMouseMove) );
    OnMouseEnter.connect (sigc::mem_fun (this, &PushButton::RecvMouseEnter) );
    OnMouseLeave.connect (sigc::mem_fun (this, &PushButton::RecvMouseLeave) );

    // Set Geometry
    SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    SetTextColor (Colors::Black);

    SetCaption (Caption);
  }

  PushButton::~PushButton()
  {
  }

  long PushButton::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void PushButton::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    InteractState is;
    is.is_on = _state;
    is.is_focus = HasMouseFocus();

    is.is_prelight = IsMouseInside();

    if (is.is_focus)
    {
      GetPainter().PushDrawSliceScaledTextureLayer (GfxContext, base, eBUTTON_FOCUS, Colors::White, eAllCorners);
      GetPainter().PopBackground();
    }
    else if (is.is_prelight)
    {
      GetPainter().PushDrawSliceScaledTextureLayer (GfxContext, base, eBUTTON_PRELIGHT, Colors::White, eAllCorners);
      GetPainter().PopBackground();
    }
    else
    {
      GetPainter().PushDrawSliceScaledTextureLayer (GfxContext, base, eBUTTON_NORMAL, Colors::White, eAllCorners);
      GetPainter().PopBackground();
    }
    GetPainter().PaintTextLineStatic (GfxContext, GetFont (), base, GetBaseString().GetTCharPtr(), GetTextColor(), true, eAlignTextCenter);
  }

  void PushButton::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void PushButton::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void PushButton::SetCaption (const TCHAR *Caption)
  {
    if (Caption == 0 || (StringLength (Caption) == 0) )
    {
      SetBaseString (TEXT ("") );
    }
    else
      SetBaseString (Caption);
  }

  const NString &PushButton::GetCaption() const
  {
    return GetBaseString();
  }

  void PushButton::SetState (bool b)
  {

  }

  void PushButton::SetState (bool State, bool EmitSignal)
  {

  }

  bool PushButton::GetState() const
  {
    return false;
  }

  void PushButton::RecvClick (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    sigClick.emit();
    NeedRedraw();
  }

  void PushButton::RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void PushButton::RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void PushButton::RecvMouseMove (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {

  }

  void PushButton::RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void PushButton::RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }
}
