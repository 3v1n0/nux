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
#include "Button.h"
#include "HLayout.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE (Button);

  Button::Button (const TCHAR *Caption, NUX_FILE_LINE_DECL)
    :   AbstractButton (Caption, NUX_FILE_LINE_PARAM)
  {
    _state     = false;

    // Set Signals
    OnMouseClick.connect (sigc::mem_fun (this, &Button::RecvClick) );
    OnMouseDown.connect (sigc::mem_fun (this, &Button::RecvMouseDown) );
    OnMouseDoubleClick.connect (sigc::mem_fun (this, &Button::RecvMouseDown) );
    OnMouseUp.connect (sigc::mem_fun (this, &Button::RecvMouseUp) );
    OnMouseMove.connect (sigc::mem_fun (this, &Button::RecvMouseMove) );
    OnMouseEnter.connect (sigc::mem_fun (this, &Button::RecvMouseEnter) );
    OnMouseLeave.connect (sigc::mem_fun (this, &Button::RecvMouseLeave) );

    // Set Geometry
    SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    SetTextColor (Colors::Black);

    SetCaption (Caption);
  }

  Button::~Button()
  {
  }

  long Button::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void Button::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    if (GetFocused ())
    {
      GetPainter().PushDrawSliceScaledTextureLayer (GfxContext, base, eBUTTON_FOCUS, Colors::White, eAllCorners);
      GetPainter().PopBackground();
    }
    else if (this->state == NUX_)
    {
      GetPainter().PushDrawSliceScaledTextureLayer (GfxContext, base, eBUTTON_PRELIGHT, Colors::White, eAllCorners);
      GetPainter().PopBackground();
    }
    else
    {
      GetPainter().PushDrawSliceScaledTextureLayer (GfxContext, base, eBUTTON_NORMAL, Colors::White, eAllCorners);
      GetPainter().PopBackground();
    }

    NString nstring_label = NString(this->label.c_str());
    GetPainter().PaintTextLineStatic (GfxContext, GetFont (), base, nstring_label, GetTextColor(), true, eAlignTextCenter);
  }

  void Button::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void Button::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void Button::SetState (bool b)
  {
    _state = b;
    NeedRedraw();
  }

}
