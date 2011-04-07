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


#include "../NuxCore.h"
#include "Complex.h"

namespace nux
{

  ComplexNumber::ComplexNumber (t_float re, t_float im)
  {
    real_ = re;
    imaginary_ = im;
  }

  ComplexNumber::ComplexNumber (const ComplexNumber &complex)
  {
    real_ = complex.real_;
    imaginary_ = complex.imaginary_;
  }

  ComplexNumber::~ComplexNumber()
  {
  }

  ComplexNumber &ComplexNumber::operator= (const ComplexNumber &complex)
  {
    real_ = complex.real_;
    imaginary_ = complex.imaginary_;

    return *this;
  }

  /*const ComplexNumber ComplexNumber::operator + (const ComplexNumber& complex) const
  {
      ComplexNumber result;

      result.real_ = real_ + complex.real_;
      result.imaginary_ = imaginary_ + complex.imaginary_;

      return result;
  }

  const ComplexNumber ComplexNumber::operator - (const ComplexNumber& complex) const
  {
      ComplexNumber result;

      result.real_ = real_ - complex.real_;
      result.imaginary_ = imaginary_ - complex.imaginary_;

      return result;
  }

  const ComplexNumber ComplexNumber::operator*(const ComplexNumber& complex) const
  {
  	ComplexNumber result;
  	t_float a, b, c, d;

  	a = real_; b = imaginary_;
  	c = complex.real_; d = complex.imaginary_;

  	result.real_ = (a*c - b*d);
  	result.imaginary_ = (a*d + b*c);

      return result;
  }

  const ComplexNumber ComplexNumber::operator / (const ComplexNumber& complex) const
  {
      ComplexNumber result;
      t_float a, b, c, d;
      t_float inv_denominator;

      a = real_; b = imaginary_;
      c = complex.real_; d = complex.imaginary_;
      inv_denominator = (t_float) 1.0 / (c*c + d*d);

      result.real_ = (a*c + b*d) * inv_denominator;
      result.imaginary_ = (b*c - a*d) * inv_denominator;

      return result;
  }
  */
  /*const ComplexNumber ComplexNumber::operator * (const t_float& f) const
  {
      ComplexNumber result;

      result.real_ = real_ * f;
      result.imaginary_ = imaginary_ * f;

      return result;
  }*/

  /*const ComplexNumber ComplexNumber::operator / (const t_float& f) const
  {
      ComplexNumber result;

      //if(f == 0.0f)
      //    trow(Exception);

      result.real_ = real_ / f ;
      result.imaginary_ = imaginary_ / f;

      return result;
  }*/

  void ComplexNumber::operator+= (const ComplexNumber &complex)
  {
    real_ += complex.real_;
    imaginary_ += complex.imaginary_;
  }

  void ComplexNumber::operator-= (const ComplexNumber &complex)
  {
    real_ -= complex.real_;
    imaginary_ -= complex.imaginary_;
  }

  void ComplexNumber::operator*= (const ComplexNumber &complex)
  {
    ComplexNumber result;
    t_float a, b, c, d;

    a = real_;
    b = imaginary_;
    c = complex.real_;
    d = complex.imaginary_;

    real_ = (a * c - b * d);
    imaginary_ = (a * d + b * c);
  }

  void ComplexNumber::operator /= (const ComplexNumber &complex)
  {
    ComplexNumber result;
    t_float a, b, c, d;
    t_float inv_denominator;

    //if(complex.real_ == 0 && complex.imaginary_ == 0)
    //    trow(Exeption);

    a = real_;
    b = imaginary_;
    c = complex.real_;
    d = complex.imaginary_;
    inv_denominator = (t_float) 1.0 / (c * c + d * d);

    real_ = (a * c + b * d) * inv_denominator;
    imaginary_ = (b * c - a * d) * inv_denominator;
  }

  /*void ComplexNumber::operator *= (const t_float& f)
  {
      real_ *= f;
      imaginary_ *= f;
  }*/

  /*void ComplexNumber::operator/=(const t_float& f)
  {
      //if(f == 0.0f)
      //    trow(Exception);

      real_ *= (t_float)1.0 / f;
      imaginary_ *= (t_float)1.0 / f;
  }*/

  void ComplexNumber::conjugue()
  {
    imaginary_ = -imaginary_;
  }

  t_float ComplexNumber::absolute()
  {
    t_float x, y, result, temp;

    x = (t_float) std::fabs (real_);
    y = (t_float) std::fabs (imaginary_);

    if (x == 0.0)
      result = y;
    else
    {
      if (y == 0.0)
        result = x;
      else
      {
        if (x > y)
        {
          temp = y / x;
          result = x * (t_float) std::sqrt (1.0 + temp * temp);
        }
        else
        {
          temp = x / y;
          result = y * (t_float) std::sqrt (1.0 + temp * temp);
        }
      }
    }

    return result;
  }

  t_bool ComplexNumber::IsNull()
  {
    if ( (real_ == 0) && (imaginary_ == 0) )
    {
      return true;
    }

    return false;
  }

  t_float ComplexNumber::real() const
  {
    return real_;
  }

  t_float ComplexNumber::imaginary() const
  {
    return imaginary_;
  }

  void ComplexNumber::real (t_float r)
  {
    real_ = r;
  }

  void ComplexNumber::imaginary (t_float i)
  {
    imaginary_ = i;
  }

  const ComplexNumber operator + (const ComplexNumber &lhs, const ComplexNumber &rhs)
  {
    return ComplexNumber (lhs.real() + rhs.real(), lhs.imaginary() + rhs.imaginary() );
  }

  const ComplexNumber operator - (const ComplexNumber &lhs, const ComplexNumber &rhs)
  {
    return ComplexNumber (lhs.real() - rhs.real(), lhs.imaginary() - rhs.imaginary() );
  }

  const ComplexNumber operator* (const ComplexNumber &lhs, const ComplexNumber &rhs)
  {
    ComplexNumber result;
    t_float a, b, c, d;

    a = lhs.real();
    b = lhs.imaginary();
    c = rhs.real();
    d = rhs.imaginary();

    result.real (a * c - b * d);
    result.imaginary (a * d + b * c);

    return result;
  }

  const ComplexNumber operator/ (const ComplexNumber &lhs, const ComplexNumber &rhs)
  {
    ComplexNumber result;
    t_float a, b, c, d;
    t_float inv_denominator;

    a = lhs.real();
    b = lhs.imaginary();
    c = rhs.real();
    d = rhs.imaginary();
    inv_denominator = (t_float) 1.0 / (c * c + d * d);

    result.real ( (a * c + b * d) * inv_denominator);
    result.imaginary ( (b * c - a * d) * inv_denominator);
    return result;
  }

  /*fcomplex Cdiv(fcomplex a, fcomplex b)
     {
     fcomplex c;
     t_float den, r;

     if (fabs(b.r) >= fabs(b.i))
        {
        r = b.i/b.r;
        den = b.r + r*b.i;
        c.r = (a.r+r*a.i) / den;
        c.i = (a.i-r*a.r) / den;
        }
     else
        {
        r = b.r/b.i;
        den = b.i + r*b.r;
        c.r = (a.r*r+a.i) / den;
        c.i = (a.i*r-a.r) / den;
        }

     return c;
     }
  */


  /*
  fcomplex Csqrt(fcomplex z)
     {
     fcomplex c;
     t_float w;

     if ((z.r == 0.0) && (z.i == 0.0))
        {
        c.r = 0.0;
        c.i = 0.0;
        }
     else
        {
        w = sqrt( (sqrt( z.r*z.r + z.i*z.i ) + fabs(z.r)) * 0.5);
        if (z.r >= 0.0)
           {
           c.r = w;
           c.i = z.i / (2.0*w);
           }
        else
           {
           c.i = (z.i >= 0) ? w : -w;
           c.r = z.i / (2.0*c.i);
           }
        }

     return c;
     }


  fcomplex RCmul(t_float x, fcomplex a)
     {
     fcomplex c;

     c.r = x*a.r;
     c.i = x*a.i;

     return c;
     }


  fcomplex Cinv( fcomplex z)
     {
     fcomplex c;
     t_float s = 1.0 / (z.r*z.r + z.i*z.i);

     c.r = z.r * s;
     c.i = -z.i * s;

     return c;
     }
     */
}
