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


#ifndef COMPLEX_H
#define COMPLEX_H

namespace nux
{

  class ComplexNumber
  {
  public:
    ~ComplexNumber();
    ComplexNumber (float re = 0.0f, float im = 0.0f);
    ComplexNumber (const ComplexNumber &);


    ComplexNumber &operator = (const ComplexNumber &);
    //const ComplexNumber operator + (const ComplexNumber&) const;
    //const ComplexNumber operator - (const ComplexNumber&) const;
    //const ComplexNumber operator * (const ComplexNumber&) const;
    //const ComplexNumber operator / (const ComplexNumber&) const;
    //const ComplexNumber operator * (const float& f) const;
    //const ComplexNumber operator / (const float& f) const;

    void operator += (const ComplexNumber &);
    void operator -= (const ComplexNumber &);
    void operator *= (const ComplexNumber &);
    void operator /= (const ComplexNumber &);
    //void operator *= (const float& f);
    //void operator /= (const float& f);

    void conjugue();
    float absolute();
    bool IsNull();
    float real() const;
    float imaginary() const;
    void real (float r);
    void imaginary (float i);

  private:
    float real_;
    float imaginary_;
  };


// does that conflict with the operators above???
  const ComplexNumber operator+ (const ComplexNumber &lhs, const ComplexNumber &rhs);
  const ComplexNumber operator- (const ComplexNumber &lhs, const ComplexNumber &rhs);
  const ComplexNumber operator* (const ComplexNumber &lhs, const ComplexNumber &rhs);
  const ComplexNumber operator/ (const ComplexNumber &lhs, const ComplexNumber &rhs);

}

#endif // COMPLEX_H
