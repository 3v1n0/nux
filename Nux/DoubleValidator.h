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


#ifndef DOUBLEVALIDATOR_H
#define DOUBLEVALIDATOR_H

#include "Validator.h"


namespace nux
{

  class DoubleValidator : public Validator
  {
  public:

    DoubleValidator (double Minimum = -1.7E308, double Maximum = 1.7E308);
    DoubleValidator (const DoubleValidator &validator);
    DoubleValidator &operator= (const DoubleValidator &rhs);
    virtual ~DoubleValidator();

    virtual Validator *Clone()  const; //Virtual Constructor Idiom

    double Validate (double d) const;
    virtual Validator::State Validate (const TCHAR *str) const;
    virtual void Alternative (const TCHAR *str);

    void SetDecimals (int dec);
    void SetMinimum (double value);
    double GetMinimum() const;
    void SetMaximum (double value);
    double GetMaximum() const;

    NString ToString (double d);
    double ToDouble (const TCHAR *str);

  private:
    double m_Minimum;
    double m_Maximum;
    GRegex *m_reg_exp;
    int m_Decimals;
  };

}

#endif // INTEGERVALIDATOR_H
