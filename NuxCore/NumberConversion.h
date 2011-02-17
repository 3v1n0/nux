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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef NUMBERCONVERSION_H
#define NUMBERCONVERSION_H

namespace nux
{

  class NString;

//! Convert a TCHAR string to a double value.
  /*!
      @param  digit A TCHAR string.
      @return A double value translated from a TCHAR string.
  */
  t_double CharToDouble (const TCHAR *digit);

//! Convert a double to an NString.
  /*!
      @param  d A double value.
      @return An NString.
  */
  NString DoubleToChar (double d);

//! Convert a TCHAR string to a 32 bits long value.
  /*!
      @param  digit A TCHAR string.
      @return A 32 bits long value translated from a TCHAR string.
  */
  t_s32 CharToInteger (const TCHAR *digit);

//! Convert an integer to a tstring.
  /*!
      @param  value An integer value.
      @param  base  Base of the integer representation.
      @return A string.
  */
  NString IntegerToChar (int value, int base = 10);

//! Convert an string to an integer.
  /*!
      @param  digit A TCHAR string.
      @return An integer value.
  */
  t_int IntegerToChar (const TCHAR *digit);

//! Convert an Hex TCHAR string to an integer value.
  /*!
      @param  value A TCHAR string.
      @return An integer value.
  */
  t_u32 HexCharToInteger (const TCHAR *s);


}

#endif // NUMBERCONVERSION_H

