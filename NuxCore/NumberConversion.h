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


#ifndef NUMBERCONVERSION_H
#define NUMBERCONVERSION_H

namespace nux
{

//! Convert a TCHAR string to a double value.
  /*!
      @param  digit A TCHAR string.
      @return A double value translated from a TCHAR string.
  */
  double CharToDouble (const TCHAR *digit);

//! Convert a double to an std::string.
  /*!
      @param  d A double value.
      @return An std::string.
  */
  std::string DoubleToChar (double d);

//! Convert a TCHAR string to a 32 bits long value.
  /*!
      @param  digit A TCHAR string.
      @return A 32 bits long value translated from a TCHAR string.
  */
  int CharToInteger (const TCHAR *digit);

//! Convert an integer to a tstring.
  /*!
      @param  value An integer value.
      @param  base  Base of the integer representation.
      @return A string.
  */
  std::string IntegerToChar (int value, int base = 10);

//! Convert an string to an integer.
  /*!
      @param  digit A TCHAR string.
      @return An integer value.
  */
  int IntegerToChar (const TCHAR *digit);

//! Convert an Hex TCHAR string to an integer value.
  /*!
      @param  value A TCHAR string.
      @return An integer value.
  */
  unsigned int HexCharToInteger (const TCHAR *s);


}

#endif // NUMBERCONVERSION_H

