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

  CheckBox::CheckBox (std::string label, NUX_FILE_LINE_DECL)
    :   ToggleButton (label, NUX_FILE_LINE_PARAM)
  {

    // Set Geometry
    //m_CheckArea->SetMinimumSize (14, 14);
    //m_CheckArea->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT) );

    //m_TextArea->SetMinimumSize (14, 14);

  }

  CheckBox::~CheckBox()
  {
  }

  void CheckBox::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground (GfxContext, base);
    //GetPainter().PaintTextLineStatic (GfxContext, GetFont (), m_TextArea->GetGeometry(), m_TextArea->GetBaseString().GetTCharPtr(), GetTextColor(), eAlignTextLeft);
//     InteractState is;
//     is.is_on = _state;
//     is.is_focus = m_TextArea->HasMouseFocus() ||
//                   HasMouseFocus() ||
//                   m_CheckArea->HasMouseFocus();
//
//     is.is_prelight = m_TextArea->IsMouseInside()
//                      || IsMouseInside() ||
//                      m_CheckArea->IsMouseInside();

    //GetPainter().PaintCheckBox (GfxContext, GetGeometry(), is, Color (0xff000000) );
  }

}
