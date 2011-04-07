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


#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

namespace nux
{

// *****************
// *               *
// *  Exception  *
// *               *
// *****************
//
// The exception handling classes from the STL do what we want,
// so this is nothing more than a base class for any of our
// derived exceptions. I have changed the constructor to take a
// std::string object, but this gets converted before calling
// std::exception.


  class Exception : public std::exception
  {
  public:
    Exception (std::string name) throw()
      : std::exception (), name_ (name)
    {
    }

    virtual ~Exception () throw () {}

    virtual const char *what () const throw()
    {
      return name_.c_str();
    }

  protected:
    std::string name_;
  };


// ************************
// *                      *
// *  Derived Exceptions  *
// *                      *
// ************************

  class BoundsException : public Exception
  {
  public:
    BoundsException (std::string name = "")
      : Exception ("apBoundsException: " + name)
    {
    }
  };

  class NotSupportedException : public Exception
  {
  public:
    NotSupportedException (std::string name = "")
      : Exception ("NotSupportedException: " + name)
    {
    }
  };

  class DivisionByZeroException : public Exception
  {
  public:
    DivisionByZeroException (std::string name = "")
      : Exception ("DivisionByZeroException: " + name)
    {
    }
  };

  class InvalidIndexException : public Exception
  {
  public:
    InvalidIndexException (std::string name = "")
      : Exception ("InvalidIndexException: " + name)
    {
    }
  };

}

#endif // EXCEPTION_H
