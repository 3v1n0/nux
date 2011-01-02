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


#include "Nux.h"
#include "DoubleValidator.h"

namespace nux
{

  DoubleValidator::DoubleValidator (double Minimum, double Maximum)
    :   m_Minimum (Minimum)
    ,   m_Maximum (Maximum)
    ,   m_Decimals (3)
  {
    _regexp_str = "^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$";

    InitRegExp ();

    if (m_Minimum > m_Maximum)
    {
      double temp = m_Minimum;
      m_Minimum = m_Maximum;
      m_Maximum = temp;
    }
  }

  DoubleValidator::DoubleValidator (const DoubleValidator &copy)
  {
    m_Minimum   = copy.m_Minimum;
    m_Minimum   = copy.m_Maximum;
    _regexp_str = copy._regexp_str;
    InitRegExp ();
  }

  DoubleValidator &DoubleValidator::operator= (const DoubleValidator &rhs)
  {
    if (&rhs != this)
    {
      m_Minimum   = rhs.m_Minimum;
      m_Minimum   = rhs.m_Maximum;
      _regexp_str = rhs._regexp_str;
      InitRegExp ();
    }

    return *this;
  }

  DoubleValidator::~DoubleValidator()
  {
  }

  Validator *DoubleValidator::Clone()  const
  {
    return new DoubleValidator (*this);
  }

  void DoubleValidator::SetMinimum (double value)
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

  void DoubleValidator::SetMaximum (double value)
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

  double DoubleValidator::Validate (double d) const
  {
    if (d < m_Minimum)
      return m_Minimum;

    if (d > m_Maximum)
      return m_Maximum;

    return d;
  }


  Validator::State DoubleValidator::Validate (const TCHAR *str) const
  {
    if (_regexp == 0)
      return Validator::Invalid;

    int out_vector [10];
    unsigned int offset = 0;
    unsigned int len    = strlen (str);

    int rc = pcre_exec(_regexp, 0, str, len, offset, 0, out_vector, sizeof(out_vector));
    if (rc <= -1)
    {
      return Validator::Invalid;
    }

    return Validator::Acceptable;
  }

  void DoubleValidator::Alternative (const TCHAR *str)
  {
    str = TEXT ("0.0");
  }

  void DoubleValidator::SetDecimals (int dec)
  {
    m_Decimals = Clamp<int> (dec, 0, 13);
  }

  NString DoubleValidator::ToString (double d)
  {
    NString Prec = NString (TEXT ("%.") ) + NString::Printf (TEXT ("%d"), m_Decimals) + NString (TEXT ("f") );
    return NString::Printf (Prec.GetTCharPtr(), d);
  }

  double DoubleValidator::ToDouble (const TCHAR *str)
  {
    if (Validate (str) == Acceptable)
      return CharToDouble (str);
    else
      return 0.0;
  }

}
