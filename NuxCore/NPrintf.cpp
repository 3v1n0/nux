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


#include "NuxCore.h"
#include "NPrintf.h"

namespace nux
{

  NString inlPrintf (const TCHAR *Format, ...)
  {
    TCHAR Result[4096];
    GET_VARARGS (Result, 4096, NUX_ARRAY_COUNT (Result) - 1, Format);
    return NString (Result);
  }

}

// see also: http://cvs.opensolaris.org/source/xref/onnv/onnv-gate/usr/src/cmd/csh/printf.c

