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
#include "VLayout.h"
#include "EditTextBox.h"
#include "CheckBox.h"
#include "StaticText.h"

namespace nux
{
  CheckBox::CheckBox (std::string label, NUX_FILE_LINE_DECL)
        : AbstractButton (NUX_FILE_LINE_PARAM)
        , label (label) {
    togglable = true;
    Init ();
  }

  CheckBox::~CheckBox() {
  }

  void CheckBox::Init () {
    SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    state.changed.connect (sigc::mem_fun(this, &CheckBox::OnStateChanged));

    // connect up to the label signal
    label.changed.connect (sigc::mem_fun(this, &CheckBox::OnLabelChanged));

    Layout *layout = new VLayout (NUX_TRACKER_LOCATION);
    SetLayout (layout);

    RebuildLayout();
  }

  void CheckBox::OnStateChanged (int value) {
    RebuildLayout();
  }

  void CheckBox::OnLabelChanged (std::string value) {
    RebuildLayout();
  }

  void CheckBox::RebuildLayout () {
    Layout *layout = new HLayout (NUX_TRACKER_LOCATION);

    // add some padding for our checkbox draw
    layout->AddLayout (new SpaceLayout (20, 20, 20, 20), 0);

    if (label().empty () == false) {
      StaticText *text = new StaticText(TEXT (label().c_str()));
      layout->AddView (text, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
    }

    // add some padding to our button, have to do it the lame way :(
    Layout *HPadding = new HLayout (NUX_TRACKER_LOCATION);
    Layout *VPadding = new VLayout (NUX_TRACKER_LOCATION);

    HPadding->AddLayout (new nux::SpaceLayout(12,12,12,12), 0);
    VPadding->AddLayout (new nux::SpaceLayout(12,12,12,12), 0);
    VPadding->AddLayout (layout, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
    VPadding->AddLayout (new nux::SpaceLayout(12,12,12,12), 0);
    HPadding->AddLayout (VPadding, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
    HPadding->AddLayout (new nux::SpaceLayout(12,12,12,12), 0);

    // NOTE - setting the layout here, unreferences the previous one, should cause all the memory
    // to be freed
    SetLayout (HPadding);

    NeedRedraw();
  }

  void CheckBox::Draw (GraphicsEngine &GfxContext, bool force_draw) {
    Geometry base = GetGeometry();

    if (state == NUX_STATE_ACTIVE) {
      GetPainter().PushDrawSliceScaledTextureLayer (GfxContext, base, eBUTTON_FOCUS, color::White, eAllCorners);
    } else if (state == NUX_STATE_PRELIGHT) {
      GetPainter().PushDrawSliceScaledTextureLayer (GfxContext, base, eBUTTON_PRELIGHT, color::White, eAllCorners);
    } else {
      GetPainter().PushDrawSliceScaledTextureLayer (GfxContext, base, eBUTTON_NORMAL, color::White, eAllCorners);
    }

    InteractState is;
    is.is_on = active;
    is.is_focus = NUX_STATE_ACTIVE;
    is.is_prelight = NUX_STATE_PRELIGHT;

    Geometry base_state = GetGeometry();
    base_state.SetWidth(20);
    base_state.SetX(base_state.x + 12);

    GetPainter().PaintCheckBox (GfxContext, base_state, is, Color (0xff000000) );
  }

  void CheckBox::DrawContent (GraphicsEngine &GfxContext, bool force_draw) {
    nux::Geometry base = GetGeometry ();
    GfxContext.PushClippingRectangle (base);

    if (GetCompositionLayout ())
      GetCompositionLayout ()->ProcessDraw (GfxContext, force_draw);

    GfxContext.PopClippingRectangle();
  }


}
