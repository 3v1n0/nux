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


#include "Nux.h"
#include "Validator.h"

namespace nux
{

  Validator::Validator()
  {
    _regexp = 0;
  }

  Validator::~Validator()
  {

  }

  bool Validator::InitRegExp ()
  {
    const char *error;
    int   erroffset;
    _regexp = pcre_compile (
      _regexp_str.GetTCharPtr (),    /* the pattern */
      PCRE_MULTILINE,
      &error,         /* for error message */
      &erroffset,     /* for error offset */
      0);             /* use default character tables */

    if (!_regexp)
    {
      nuxDebugMsg (TEXT("[IntegerValidator::IntegerValidator] Invalid regular expression: %s"), _regexp_str);
      return false;
    }    
    return true;
  }

}
