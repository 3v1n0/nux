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


#ifndef DOUBLEVALIDATOR_H
#define DOUBLEVALIDATOR_H

#include "Validator.h"

namespace nux
{

  class DoubleValidator : public Validator
  {
  public:

    DoubleValidator(double Minimum = -1.7E308, double Maximum = 1.7E308);
    DoubleValidator(const DoubleValidator &validator);
    DoubleValidator &operator= (const DoubleValidator &rhs);
    virtual ~DoubleValidator();

    virtual Validator *Clone()  const; //Virtual Constructor Idiom

    /*!
        @return The input value clamped to the range [m_Minimum, m_Maximum].
    */
    double GetClampedValue(double d) const;

    virtual void Alternative(const char *str);

    void SetDecimals(int dec);
    void SetMinimum(double value);
    double GetMinimum() const;
    void SetMaximum(double value);
    double GetMaximum() const;

    std::string ToString(double d);
    double ToDouble(const char *str);

  private:
    double m_Minimum;
    double m_Maximum;
    int m_Decimals;
  };

}

#endif // INTEGERVALIDATOR_H
