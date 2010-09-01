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


#include "NKernel.h"
#include "NStreamBuffer.h"
#include <functional>
#include <cassert>
#include <cstring>

NAMESPACE_BEGIN

NStreamBuffer::NStreamBuffer(const BYTE *begin, const BYTE *end) :
begin_(begin),
end_(end),
current_(begin_)
{
    assert(std::less_equal<const BYTE *>()(begin_, end_));
}

NStreamBuffer::NStreamBuffer(const BYTE *str, int size) :
begin_(str),
end_(begin_ + size),
current_(begin_)
{
}

NStreamBuffer::~NStreamBuffer()
{
    delete begin_;
}

NStreamBuffer::int_type NStreamBuffer::underflow()
{
    if (current_ == end_)
        return traits_type::eof();

    return *current_;
}

NStreamBuffer::int_type NStreamBuffer::uflow()
{
    if (current_ == end_)
        return traits_type::eof();

    return *current_++;
}

NStreamBuffer::int_type NStreamBuffer::pbackfail(int_type ch)
{
    if (current_ == begin_ || (ch != traits_type::eof() && ch != current_[-1]))
        return traits_type::eof();

    return *--current_;
}

std::streamsize NStreamBuffer::showmanyc()
{
    assert(std::less_equal<const BYTE *>()(current_, end_));
    return end_ - current_;
}

NAMESPACE_END