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
#include "ScrollBar.h"

namespace nux
{

  ScrollBar::ScrollBar(NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
  {
    m_visible   = 1;
    m_ScrollUnit = 5;
    visibility_percentage_ = 0.0f;
  }

  ScrollBar::~ScrollBar()
  {
  }

  void ScrollBar::SetScrollUnit(int unit)
  {
    nuxAssert(unit > 0);
    m_ScrollUnit = unit;

    if (m_ScrollUnit <= 0)
      m_ScrollUnit = 5;
  }

  int ScrollBar::GetScrollUnit() const
  {
    return m_ScrollUnit;
  }

  bool ScrollBar::AcceptKeyNavFocus()
  {
    return false;
  }

}
