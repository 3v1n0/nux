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


#ifndef NSTREAMBUFFER_H
#define NSTREAMBUFFER_H

#include <streambuf>

namespace nux
{
  class NStreamBuffer : public std::streambuf
  {
  public:
    NStreamBuffer (const BYTE *begin, const BYTE *end);
    explicit NStreamBuffer (const BYTE *str, int size);

    ~NStreamBuffer();

  private:
    int_type uflow();
    int_type underflow();
    int_type pbackfail (int_type ch);
    std::streamsize showmanyc();

    // copy ctor and assignment not implemented;
    // copying not allowed
    NStreamBuffer (const NStreamBuffer &);
    NStreamBuffer &operator= (const NStreamBuffer &);

  private:
    const BYTE *const begin_;
    const BYTE *const end_;
    const BYTE *current_;
  };

}

#endif // NSTREAMBUFFER_H