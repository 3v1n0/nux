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


#include "Nux.h"
#include "Validator.h"

namespace nux
{

  Validator::Validator()
  {
  }

  Validator::~Validator()
  {

  }

  bool Validator::InitRegExp()
  {
#if defined(NUX_OS_WINDOWS)
    regex_ = _regexp_str.c_str();
    return true;
#else
    const char *error;
    int   erroffset;
    _regexp = pcre_compile(
      _regexp_str.c_str(),    /* the pattern */
      PCRE_MULTILINE,
      &error,         /* for error message */
      &erroffset,     /* for error offset */
      0);             /* use default character tables */

    if (!_regexp)
    {
      nuxDebugMsg("[IntegerValidator::IntegerValidator] Invalid regular expression: %s", _regexp_str.c_str());
      return false;
    }    
    return true;
#endif
  }

  Validator::State Validator::Validate(const char* str) const
  {
#if defined(NUX_OS_WINDOWS)
    if (str == NULL)
      return Validator::Invalid;
    std::string search_string = str;

    if (std::regex_match(search_string.begin(), search_string.end(), regex_))
    {
      return Validator::Acceptable;
    }
    return Validator::Acceptable;
#else
    if (_regexp == 0)
      return Validator::Invalid;

    int out_vector [10];
    unsigned int offset = 0;
    int len = (int) strlen(str);

    // See the "PCRE DISCUSSION OF STACK USAGE" and why it maybe necessary to limit the stack usage.
    pcre_extra extra;
    extra.flags = PCRE_EXTRA_MATCH_LIMIT_RECURSION;
    extra.match_limit_recursion = 2000; 

    int rc = pcre_exec(_regexp, &extra, str, len, offset, 0, out_vector, 10);
    if (rc <= -1)
    {
      return Validator::Invalid;
    }
    return Validator::Acceptable;
#endif
  }

}
