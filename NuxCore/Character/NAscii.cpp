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
#include "NAscii.h"

namespace nux
{

// Windows-1252 code page
  t_uchar GAscii[] =
  {
    0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,    0x8,    0x9,    0xA,    0xB,    0xC,    0xD,    0xE,    0xF,
    0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,   0x18,   0x19,   0x1A,   0x1B,   0x1C,   0x1D,   0x1E,   0x1F,
    ' ',    '!',    '"',    '#',    '$',    '%',    '&',    '\'',   '(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',
    '0',    '1',    '2',    '3',    '4',    '5',    '6',    '7',    '8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',
    '@',    'A',    'B',    'C',    'D',    'E',    'F',    'G',    'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',
    'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',    'X',    'Y',    'Z',    '[',    '\\',   ']',    '^',    '_',
    '`',    'a',    'b',    'c',    'd',    'e',    'f',    'g',    'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',
    'p',    'q',    'r',    's',    't',    'u',    'v',    'w',    'x',    'y',    'z',    '{',    '|',    '}',    '~',    0xFF,
    '€',    0x81,   '‚',    'ƒ',    '„',    '…',    '†',    '‡',    'ˆ',    '‰',    'Š',    '‹',    'Œ',    0x8d,   'Ž',    0x8f,
    0x90,   '‘',    '’',    '“',    '”',    '•',    '–',    '—',    0x98,   '™',    'š',    '›',    'œ',    0x9d,   'ž',    'Ÿ',
    0xa0,   '¡',    '¢',    '£',    '¤',    '¥',    '¦',    '§',    '¨',    '©',    'ª',    '«',    '¬',    '­',    '®',    '¯',
    '°',    '±',    '²',    '³',    '´',    'µ',    '¶',    '·',    '¸',    '¹',    'º',    '»',    '¼',    '½',    '¾',    '¿',
    'À',    'Á',    'Â',    'Ã',    'Ä',    'Å',    'Æ',    'Ç',    'È',    'É',    'Ê',    'Ë',    'Ì',    'Í',    'Î',    'Ï',
    'Ð',    'Ñ',    'Ò',    'Ó',    'Ô',    'Õ',    'Ö',    '×',    'Ø',    'Ù',    'Ú',    'Û',    'Ü',    'Ý',    'Þ',    'ß',
    'à',    'á',    'â',    'ã',    'ä',    'å',    'æ',    'ç',    'è',    'é',    'ê',    'ë',    'ì',    'í',    'î',    'ï',
    'ð',    'ñ',    'ò',    'ó',    'ô',    'õ',    'ö',    '÷',    'ø',    'ù',    'ú',    'û',    'ü',    'ý',    'þ',    'ÿ'
  };


}
