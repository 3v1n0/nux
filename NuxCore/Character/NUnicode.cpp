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


#include "NuxCore.h"
#include "NUnicode.h"


namespace nux
{

  ANSICHAR *UnicharToAnsicharConvertion::Convert (const UNICHAR *Source)
  {
    std::wstring utf16string (Source);
    size_t utf16size = utf16string.length();
    size_t utf8size = 6 * utf16size;
    ANSICHAR *utf8string = new ANSICHAR[utf8size+1];

    const wchar_t *source_start = utf16string.c_str();
    const wchar_t *source_end = source_start + utf16size;
    unsigned char *target_start = reinterpret_cast<unsigned char *> (utf8string);
    unsigned char *target_end = target_start + utf8size;

    ConversionResult res = ConvertUTF16toUTF8 (&source_start, source_end, &target_start, target_end, lenientConversion);

    if (res != conversionOK)
    {
      delete [] utf8string;
      utf8string = 0;
    }

    // mark end of string
    *target_start = 0;
    return utf8string;
  }

  UNICHAR *AnsicharToUnicharConvertion::Convert (const ANSICHAR *Source)
  {
    std::string utf8string (Source);
    size_t utf8size = utf8string.length();
    size_t utf16size = utf8size;
    UNICHAR *utf16string = new UNICHAR[utf16size+1];

    const unsigned char *source_start = reinterpret_cast<const unsigned char *> (utf8string.c_str() );
    const unsigned char *source_end = source_start + utf8size;
    wchar_t *target_start = reinterpret_cast<wchar_t *> (utf16string);
    wchar_t *target_end = target_start + utf16size;

    ConversionResult res = ConvertUTF8toUTF16 (&source_start, source_end, &target_start, target_end, lenientConversion);

    if (res != conversionOK)
    {
      delete[] utf16string;
      utf16string = 0;
    }

    // mark end of string
    *target_start = 0;
    return utf16string;
  }

}
