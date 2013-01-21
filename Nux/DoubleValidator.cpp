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
#include "DoubleValidator.h"

#include <sstream>
#include <iomanip>

namespace nux
{

  DoubleValidator::DoubleValidator(double Minimum, double Maximum)
    :   m_Minimum(Minimum)
    ,   m_Maximum(Maximum)
    ,   m_Decimals(3)
  {
    _regexp_str = "^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$";

    InitRegExp();

    if (m_Minimum > m_Maximum)
    {
      double temp = m_Minimum;
      m_Minimum = m_Maximum;
      m_Maximum = temp;
    }
  }

  DoubleValidator::DoubleValidator(const DoubleValidator &copy)
  {
    m_Minimum   = copy.m_Minimum;
    m_Maximum   = copy.m_Maximum;
    _regexp_str = copy._regexp_str;
    InitRegExp();
  }

  DoubleValidator &DoubleValidator::operator= (const DoubleValidator &rhs)
  {
    if (&rhs != this)
    {
      m_Minimum   = rhs.m_Minimum;
      m_Maximum   = rhs.m_Maximum;
      _regexp_str = rhs._regexp_str;
      InitRegExp();
    }

    return *this;
  }

  DoubleValidator::~DoubleValidator()
  {
  }

  Validator *DoubleValidator::Clone()  const
  {
    return new DoubleValidator(*this);
  }

  void DoubleValidator::SetMinimum(double value)
  {
    m_Minimum = value;

    if (m_Minimum > m_Maximum)
    {
      double temp = m_Minimum;
      m_Minimum = m_Maximum;
      m_Maximum = temp;
    }
  }

  double DoubleValidator::GetMinimum() const
  {
    return m_Minimum;
  }

  void DoubleValidator::SetMaximum(double value)
  {
    m_Maximum = value;

    if (m_Minimum > m_Maximum)
    {
      double temp = m_Minimum;
      m_Minimum = m_Maximum;
      m_Maximum = temp;
    }
  }

  double DoubleValidator::GetMaximum() const
  {
    return m_Maximum;
  }

  double DoubleValidator::GetClampedValue(double d) const
  {
    if (d < m_Minimum)
      return m_Minimum;

    if (d > m_Maximum)
      return m_Maximum;

    return d;
  }

  void DoubleValidator::Alternative(const char * /* str */)
  {
  }

  void DoubleValidator::SetDecimals(int dec)
  {
    m_Decimals = Clamp<int> (dec, 0, 13);
  }

  std::string DoubleValidator::ToString(double d)
  {
    std::stringstream s;
    s << std::setprecision(m_Decimals) << d;
    return s.str();
  }

  double DoubleValidator::ToDouble(const char *str)
  {
    if (Validate(str) == Acceptable)
      return CharToDouble(str);
    else
      return 0.0;
  }

}
