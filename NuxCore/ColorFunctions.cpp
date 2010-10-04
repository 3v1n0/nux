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


#include "NKernel.h"
#include "NSystemTypes.h"
#include "ColorFunctions.h"


namespace nux   //NUX_NAMESPACE_BEGIN
{
// Converts from Gamma2.2 space to Gamma1.0 space
  t_float Undo_Gamma_Correction (t_float c)
  {
    if (c <= 0.03928f)
    {
      return (c / 12.92f);
    }
    else
    {
      return (float) pow ( (c + 0.055f) / 1.055f, 2.4f);
    }
  }

  t_float Do_Gamma_Correction (t_float c)
  {
    if (c <= 0.00304f)
    {
      return (c * 12.92f);
    }
    else
    {
      return (float) pow (c, 1.0f / 2.4f) * 1.055f - 0.055f;
    }
  }

} //NUX_NAMESPACE_END