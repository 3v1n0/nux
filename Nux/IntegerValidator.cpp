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
#include "IntegerValidator.h"

namespace nux
{

  IntegerValidator::IntegerValidator(int Minimum, int Maximum)
    :   m_Minimum(Minimum)
    ,   m_Maximum(Maximum)
  {
    _regexp_str = "^[-+]?[0-9]+$";

    InitRegExp();

    if (m_Minimum > m_Maximum)
    {
      int temp = m_Minimum;
      m_Minimum = m_Maximum;
      m_Maximum = temp;
    }
  }

  IntegerValidator::IntegerValidator(const IntegerValidator &copy)
  {
    m_Minimum   = copy.m_Minimum;
    m_Maximum   = copy.m_Maximum;
    _regexp_str    = copy._regexp_str;
    InitRegExp();
  }

  IntegerValidator &IntegerValidator::operator= (const IntegerValidator &rhs)
  {
    if (&rhs != this)
    {
      m_Minimum   = rhs.m_Minimum;
      m_Maximum   = rhs.m_Maximum;
      _regexp_str    = rhs._regexp_str;
      InitRegExp();
    }

    return *this;
  }

  IntegerValidator::~IntegerValidator()
  {

  }

  Validator *IntegerValidator::Clone()  const
  {
    return new IntegerValidator(*this);
  }

  void IntegerValidator::SetMinimum(int value)
  {
    m_Minimum = value;

    if (m_Minimum > m_Maximum)
    {
      int temp = m_Minimum;
      m_Minimum = m_Maximum;
      m_Maximum = temp;
    }
  }

  int IntegerValidator::GetMinimum() const
  {
    return m_Minimum;
  }

  void IntegerValidator::SetMaximum(int value)
  {
    m_Maximum = value;

    if (m_Minimum > m_Maximum)
    {
      int temp = m_Minimum;
      m_Minimum = m_Maximum;
      m_Maximum = temp;
    }
  }

  int IntegerValidator::GetMaximum() const
  {
    return m_Maximum;
  }

  int IntegerValidator::GetClampedValue(int i) const
  {
    if (i < m_Minimum)
      return m_Minimum;

    if (i > m_Maximum)
      return m_Maximum;

    return i;
  }

  void IntegerValidator::Alternative(const char * /* str */)
  {
  }

  std::string IntegerValidator::ToString(int i)
  {
    return std::to_string((long long)i);
  }

  int IntegerValidator::ToInteger(const char *str)
  {
    if (Validate(str) == Acceptable)
      return CharToInteger(str);
    else
      return 0;
  }
}

