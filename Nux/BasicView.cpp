/*
 * Copyright 2012 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#include "Nux/Nux.h"
#include "BasicView.h"


namespace nux {
NUX_IMPLEMENT_OBJECT_TYPE(BasicView);

BasicView::BasicView(NUX_FILE_LINE_DECL)
: nux::View(NUX_FILE_LINE_PARAM)
{

}

BasicView::~BasicView()
{

}

void BasicView::Draw(nux::GraphicsEngine& /* graphics_engine */, bool /* force_draw */)
{
  // Draw nothing
  // For debug only:
  // graphics_engine.QRP_Color(GetBaseX(), GetBaseY(), GetBaseWidth(), GetBaseHeight(), nux::color::Pink);
}

}
