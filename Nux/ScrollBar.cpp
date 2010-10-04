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


#include "Nux.h"
#include "ScrollBar.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

  ScrollBar::ScrollBar (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    m_visible   = 1;
    m_ScrollUnit = 5;
  }

  ScrollBar::~ScrollBar()
  {
  }

  void ScrollBar::SetScrollUnit (int unit)
  {
    nuxAssert (unit > 0);
    m_ScrollUnit = unit;

    if (m_ScrollUnit <= 0)
      m_ScrollUnit = 5;
  }

  int ScrollBar::GetScrollUnit() const
  {
    return m_ScrollUnit;
  }


} //NUX_NAMESPACE_END
