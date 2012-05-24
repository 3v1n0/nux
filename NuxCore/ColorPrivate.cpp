/*
 * Copyright 2012 Inalogic® Inc.
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
 * Authored by: Andrea Azzarone <azzaronea@gmail.com>
 *
 */

#include <sstream>

#include "ColorPrivate.h"

namespace nux
{
namespace color
{
namespace impl
{

bool IsValidHex(std::string const& hex)
{
  static const std::string allowed_chars("#0123456789abcdefABCDEF");

  if (hex.empty())
    return false;
  else if (hex[0] == '#' && hex.length() != 7)
    return false;
  else if (hex[0] != '#' && hex.length() != 6)
    return false;
  else
    return (hex.find_first_not_of(allowed_chars) == hex.npos);
}


int HexToDec(std::string const& hex)
{
  std::istringstream istr(hex);
  int ret;
  istr >> std::hex >> ret;

  return ret;
}


bool HexToRGB(std::string hex, float& r, float& g, float& b)
{
  if (!IsValidHex(hex))
    return false;

  if (hex[0] == '#')
    hex.erase(hex.begin());

  std::string red(hex.substr(0, 2));
  std::string green(hex.substr(2, 2));
  std::string blue(hex.substr(4, 2));

  r = HexToDec(red) / 255.0f;
  g = HexToDec(green) / 255.0f;
  b = HexToDec(blue) / 255.0f;

  return true;
}

} // namespace impl
} // namespace color
} // namespace nux

