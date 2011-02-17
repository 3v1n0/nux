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


#ifndef HEXREGEXPVALIDATOR_H
#define HEXREGEXPVALIDATOR_H

#include "Validator.h"

namespace nux
{

  class HexRegExpValidator : public Validator
  {
  public:
    HexRegExpValidator (int Minimum = INT_MIN, int Maximum = INT_MAX);
    HexRegExpValidator (const HexRegExpValidator &validator);
    HexRegExpValidator &operator= (const HexRegExpValidator &rhs);
    virtual ~HexRegExpValidator();

    virtual Validator *Clone()  const; //Virtual Constructor Idiom

    /*!
        @return The input value clamped to the range [m_Minimum, m_Maximum].
    */
    int GetClampedValue (int i) const;

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
  };
}

#endif // HEXREGEXPVALIDATOR_H

