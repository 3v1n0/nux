// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2011 Inalogic® Inc.
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
 * Authored by: Tim Penhey <tim.penhey@canonical.com>
 *
 */

#include "AsyncFileWriter.h"

namespace nux
{

class AsyncFileWriter::Impl
{
public:
  Impl(std::string const& filename);
  ~Impl();

  void Write(std::string const& data);
  void Close(Callback file_closed_callback);
private:
  std::string filename_;
};


AsyncFileWriter::Impl::Impl(std::string const& filename)
{
}

AsyncFileWriter::Impl::~Impl()
{
}

void AsyncFileWriter::Impl::Write(std::string const& data)
{
}

void AsyncFileWriter::Impl::Close(Callback file_closed_callback)
{
}

AsyncFileWriter::AsyncFileWriter(std::string const& filename)
  : pimpl(new Impl(filename))
{}

AsyncFileWriter::~AsyncFileWriter()
{
  delete pimpl;
}

void AsyncFileWriter::Write(std::string const& data)
{
  pimpl->Write(data);
}

void AsyncFileWriter::Close(Callback file_closed_callback)
{
  pimpl->Close(file_closed_callback);
}



} // namespace nux
