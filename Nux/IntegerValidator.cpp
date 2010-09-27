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
#include "IntegerValidator.h"

namespace nux { //NUX_NAMESPACE_BEGIN

IntegerValidator::IntegerValidator(int Minimum, int Maximum)
:   m_Minimum(Minimum)
,   m_Maximum(Maximum)
{
    //m_RegExp = TEXT("[-+]?[0-9]+");
    if(m_Minimum > m_Maximum)
    {
        int temp = m_Minimum;
        m_Minimum = m_Maximum;
        m_Maximum = temp;
    }
}

IntegerValidator::IntegerValidator(const IntegerValidator& copy)
{
    m_Minimum   = copy.m_Minimum;
    m_Minimum   = copy.m_Maximum;
    //m_RegExp    = copy.m_RegExp; // boost::regex has an assignment constructor defined.
}

IntegerValidator& IntegerValidator::operator=(const IntegerValidator& rhs)
{
    if(&rhs != this)
    {
        m_Minimum   = rhs.m_Minimum;
        m_Minimum   = rhs.m_Maximum;
        //m_RegExp    = rhs.m_RegExp; // boost::regex has an assignment constructor defined.
    }
    return *this;
}

IntegerValidator::~IntegerValidator()
{

}

Validator* IntegerValidator::Clone()  const
{
    return new IntegerValidator(*this);
}

void IntegerValidator::SetMinimum(int value)
{
    m_Minimum = value;
    if(m_Minimum > m_Maximum)
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
    if(m_Minimum > m_Maximum)
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

int IntegerValidator::Validate(int i) const
{
    if(i < m_Minimum)
        return m_Minimum;
    if(i > m_Maximum)
        return m_Maximum;
    return i;
}

Validator::State IntegerValidator::Validate(const TCHAR* str) const
{
  /*
    if (!boost::regex_match(str, m_RegExp))
    {
        return Validator::Invalid;
    }
*/
    return Validator::Acceptable;
}

void IntegerValidator::Alternative(const TCHAR* str)
{
    str = TEXT("0");
}

NString IntegerValidator::ToString(int i)
{
    NString Prec(TEXT("%d"));
    return inlPrintf(Prec.GetTCharPtr(), i);
}

int IntegerValidator::ToInteger(const TCHAR* str)
{
    if(Validate(str) == Acceptable)
        return CharToInteger(str);
    else
        return 0;
}



} //NUX_NAMESPACE_END
