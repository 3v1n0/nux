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

#include "AbstractSeparator.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

  AbstractSeparator::AbstractSeparator (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_Color (0xFF888888)
    ,   m_Alpha0 (0.0f)
    ,   m_Alpha1 (0.592f)
    ,   m_BorderSize (10)
  {

  }

  AbstractSeparator::AbstractSeparator (const Color &color, float Alpha0, float Alpha1, int Border, NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_Color (color)
    ,   m_Alpha0 (Alpha0)
    ,   m_Alpha1 (Alpha1)
    ,   m_BorderSize (Border)
  {

  }

  AbstractSeparator::~AbstractSeparator()
  {

  }

  void AbstractSeparator::SetColor (const Color &color)
  {
    m_Color = color;
  }

  void AbstractSeparator::SetAlpha (float Alpha0, float Alpha1)
  {
    m_Alpha0 = Alpha0;
    m_Alpha1 = Alpha1;
  }

  void AbstractSeparator::SetBorderSize (int Border)
  {
    m_BorderSize = Border;
  }
} //NUX_NAMESPACE_END
