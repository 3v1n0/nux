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

#ifndef INTEGERVALIDATOR_H
#define INTEGERVALIDATOR_H

#include "Validator.h"

namespace nux
{

  class IntegerValidator : public Validator
  {
  public:
    IntegerValidator(int Minimum = INT_MIN, int Maximum = INT_MAX);
    IntegerValidator(const IntegerValidator &validator);
    IntegerValidator &operator= (const IntegerValidator &rhs);
    virtual ~IntegerValidator();

    virtual Validator *Clone()  const; //Virtual Constructor Idiom

    /*!
        @return The input value clamped to the range [m_Minimum, m_Maximum].
    */
    int GetClampedValue(int i) const;

    virtual void Alternative(const char *str);

    void SetMinimum(int value);
    int GetMinimum() const;
    void SetMaximum(int value);
    int GetMaximum() const;

    std::string ToString(int i);
    int ToInteger(const char *str);

  private:
    int m_Minimum;
    int m_Maximum;
  };
}

#endif // INTEGERVALIDATOR_H
