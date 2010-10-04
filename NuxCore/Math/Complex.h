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


#ifndef COMPLEX_H
#define COMPLEX_H

namespace nux   //NUX_NAMESPACE_BEGIN
{

  class Complex
  {
  public:
    ~Complex();
    Complex (t_float re = 0.0f, t_float im = 0.0f);
    Complex (const Complex &);


    Complex &operator = (const Complex &);
    //const Complex operator + (const Complex&) const;
    //const Complex operator - (const Complex&) const;
    //const Complex operator * (const Complex&) const;
    //const Complex operator / (const Complex&) const;
    //const Complex operator * (const t_float& f) const;
    //const Complex operator / (const t_float& f) const;

    void operator += (const Complex &);
    void operator -= (const Complex &);
    void operator *= (const Complex &);
    void operator /= (const Complex &);
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
  const Complex operator+ (const Complex &lhs, const Complex &rhs);
  const Complex operator- (const Complex &lhs, const Complex &rhs);
  const Complex operator* (const Complex &lhs, const Complex &rhs);
  const Complex operator/ (const Complex &lhs, const Complex &rhs);

} //NUX_NAMESPACE_END

#endif // COMPLEX_H
