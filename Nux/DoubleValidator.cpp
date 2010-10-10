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
#include "DoubleValidator.h"

namespace nux
{

  DoubleValidator::DoubleValidator (double Minimum, double Maximum)
    :   m_Minimum (Minimum)
    ,   m_Maximum (Maximum)
    ,   m_Decimals (3)
  {
    m_reg_exp = g_regex_new ("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?", G_REGEX_CASELESS, G_REGEX_MATCH_ANCHORED, NULL);

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
    m_reg_exp    = copy.m_reg_exp;
    g_regex_ref (m_reg_exp);
  }

  DoubleValidator &DoubleValidator::operator= (const DoubleValidator &rhs)
  {
    if (&rhs != this)
    {
      m_Minimum   = rhs.m_Minimum;
      m_Minimum   = rhs.m_Maximum;
      m_reg_exp    = rhs.m_reg_exp;
      g_regex_ref (m_reg_exp);
    }

    return *this;
  }

  DoubleValidator::~DoubleValidator()
  {
    g_regex_unref (m_reg_exp);
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
    GMatchInfo *match_info;

    if (!g_regex_match (m_reg_exp, str, G_REGEX_MATCH_ANCHORED, &match_info) )
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
    NString Prec = NString (TEXT ("%.") ) + inlPrintf (TEXT ("%d"), m_Decimals) + NString (TEXT ("f") );
    return inlPrintf (Prec.GetTCharPtr(), d);
  }

  double DoubleValidator::ToDouble (const TCHAR *str)
  {
    if (Validate (str) == Acceptable)
      return CharToDouble (str);
    else
      return 0.0;
  }

}
