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


#ifndef VALIDATOR_H
#define VALIDATOR_H

#if defined (NUX_OS_WINDOWS)
  #include "pcre/pcre.h"
#else
  #include <pcre.h>
#endif

namespace nux
{

  class Validator
  {
  public:
    enum State
    {
      Invalid         = 0,
      Intermediate,
      Acceptable
    };

    Validator();
    virtual ~Validator();

    virtual Validator *Clone()  const = 0; //Virtual Constructor Idiom

    virtual Validator::State Validate (const TCHAR *str) const;
    virtual void Alternative (const TCHAR *str) {}

  protected:
    bool InitRegExp ();
    NString _regexp_str;
    pcre *_regexp;
  };
}

#endif // VALIDATOR_H
