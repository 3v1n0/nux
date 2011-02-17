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


#ifndef COMPLEX_H
#define COMPLEX_H

namespace nux
{

  class ComplexNumber
  {
  public:
    ~ComplexNumber();
    ComplexNumber (t_float re = 0.0f, t_float im = 0.0f);
    ComplexNumber (const ComplexNumber &);


    ComplexNumber &operator = (const ComplexNumber &);
    //const ComplexNumber operator + (const ComplexNumber&) const;
    //const ComplexNumber operator - (const ComplexNumber&) const;
    //const ComplexNumber operator * (const ComplexNumber&) const;
    //const ComplexNumber operator / (const ComplexNumber&) const;
    //const ComplexNumber operator * (const t_float& f) const;
    //const ComplexNumber operator / (const t_float& f) const;

    void operator += (const ComplexNumber &);
    void operator -= (const ComplexNumber &);
    void operator *= (const ComplexNumber &);
    void operator /= (const ComplexNumber &);
    //void operator *= (const t_float& f);
    //void operator /= (const t_float& f);

    void conjugue();
    t_float absolute();
    t_bool IsNull();
    t_float real() const;
    t_float imaginary() const;
    void real (t_float r);
    void imaginary (t_float i);

  private:
    t_float real_;
    t_float imaginary_;
  };


// does that conflict with the operators above???
  const ComplexNumber operator+ (const ComplexNumber &lhs, const ComplexNumber &rhs);
  const ComplexNumber operator- (const ComplexNumber &lhs, const ComplexNumber &rhs);
  const ComplexNumber operator* (const ComplexNumber &lhs, const ComplexNumber &rhs);
  const ComplexNumber operator/ (const ComplexNumber &lhs, const ComplexNumber &rhs);

}

#endif // COMPLEX_H
