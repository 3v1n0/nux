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
#include "HexRegExpValidator.h"

namespace nux
{

  HexRegExpValidator::HexRegExpValidator (int Minimum, int Maximum)
    :   m_Minimum (Minimum)
    ,   m_Maximum (Maximum)
  {
    _regexp_str = "^(0[xX])*[0-9a-fA-F]+$";

    InitRegExp ();

    if (m_Minimum > m_Maximum)
    {
      int temp = m_Minimum;
      m_Minimum = m_Maximum;
      m_Maximum = temp;
    }
  }

  HexRegExpValidator::HexRegExpValidator (const HexRegExpValidator &copy)
  {
    m_Minimum   = copy.m_Minimum;
    m_Minimum   = copy.m_Maximum;
    _regexp_str = copy._regexp_str;
    InitRegExp ();
  }

  HexRegExpValidator &HexRegExpValidator::operator= (const HexRegExpValidator &rhs)
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

  HexRegExpValidator::~HexRegExpValidator()
  {
  }

  Validator *HexRegExpValidator::Clone()  const
  {
    return new HexRegExpValidator (*this);
  }

  void HexRegExpValidator::SetMinimum (int value)
  {
    m_Minimum = value;

    if (m_Minimum > m_Maximum)
    {
      int temp = m_Minimum;
      m_Minimum = m_Maximum;
      m_Maximum = temp;
    }
  }

  int HexRegExpValidator::GetMinimum() const
  {
    return m_Minimum;
  }

  void HexRegExpValidator::SetMaximum (int value)
  {
    m_Maximum = value;

    if (m_Minimum > m_Maximum)
    {
      int temp = m_Minimum;
      m_Minimum = m_Maximum;
      m_Maximum = temp;
    }
  }

  int HexRegExpValidator::GetMaximum() const
  {
    return m_Maximum;
  }

  int HexRegExpValidator::Validate (int i) const
  {
    if (i < m_Minimum)
      return m_Minimum;

    if (i > m_Maximum)
      return m_Maximum;

    return i;
  }

  Validator::State HexRegExpValidator::Validate (const TCHAR *str) const
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

  void HexRegExpValidator::Alternative (const TCHAR *str)
  {
    str = TEXT ("0x0");
  }

  NString HexRegExpValidator::ToString (int i)
  {
    NString Prec (TEXT ("%d"));
    return NString::Printf (Prec.GetTCharPtr (), i);
  }

  int HexRegExpValidator::ToInteger (const TCHAR *str)
  {
    if (Validate (str) == Acceptable)
      return HexCharToInteger (str);
    else
      return 0;
  }

}
