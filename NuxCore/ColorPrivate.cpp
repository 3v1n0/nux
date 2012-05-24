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

bool IsValidHex(std::string hex)
{
  static const std::string allowed_chars("0123456789abcdefABCDEF");

  if (hex.empty())
    return false;

  if (hex[0] == '#')
    hex.erase(hex.begin());

  switch (hex.length())
  {
    case 3: // #rgb
    case 4: // #rgba
    case 6: // #rrggbb
    case 8: // #rrggbbaa
      return (hex.find_first_not_of(allowed_chars) == hex.npos);
    default:
      return false;
  }
}


int HexToInt(std::string const& hex)
{
  std::istringstream istr(hex);
  int ret;
  istr >> std::hex >> ret;

  return ret;
}


bool HexToRGBA(std::string hex, float& r, float& g, float& b, float& a)
{
  if (!IsValidHex(hex))
    return false;

  if (hex[0] == '#')
    hex.erase(hex.begin());

  a = 1.0f;

  if (hex.length() == 3 || hex.length() == 4)
  {
    r = HexToInt(hex.substr(0, 1) + hex.substr(0, 1)) / 255.0f;
    g = HexToInt(hex.substr(1, 1) + hex.substr(1, 1)) / 255.0f;
    b = HexToInt(hex.substr(2, 1) + hex.substr(2, 1)) / 255.0f;

    if (hex.length() == 4)
      a = HexToInt(hex.substr(3, 1) + hex.substr(3, 1)) / 255.0f;
  }
  else if (hex.length() == 6 || hex.length() == 8)
  {
    r = HexToInt(hex.substr(0, 2)) / 255.0f;
    g = HexToInt(hex.substr(2, 2)) / 255.0f;
    b = HexToInt(hex.substr(4, 2)) / 255.0f;

    if (hex.length() == 8)
      a = HexToInt(hex.substr(6, 2)) / 255.0f;
  }

  return true;
}

} // namespace color
} // namespace nux

