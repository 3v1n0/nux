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

#ifndef INTEGERVALIDATOR_H
#define INTEGERVALIDATOR_H

#include "Validator.h"

namespace nux
{

  class IntegerValidator : public Validator
  {
  public:
    IntegerValidator (int Minimum = INT_MIN, int Maximum = INT_MAX);
    IntegerValidator (const IntegerValidator &validator);
    IntegerValidator &operator= (const IntegerValidator &rhs);
    virtual ~IntegerValidator();

    virtual Validator *Clone()  const; //Virtual Constructor Idiom

    int Validate (int i) const;
    virtual Validator::State Validate (const TCHAR *str) const;
    virtual void Alternative (const TCHAR *str);

    void SetMinimum (int value);
    int GetMinimum() const;
    void SetMaximum (int value);
    int GetMaximum() const;

    NString ToString (int i);
    int ToInteger (const TCHAR *str);

  private:
    int m_Minimum;
    int m_Maximum;
    GRegex *m_reg_exp;
  };
}

#endif // INTEGERVALIDATOR_H
