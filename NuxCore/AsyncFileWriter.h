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
#ifndef NUXCORE_ASYNC_FILE_WRITER_H
#define NUXCORE_ASYNC_FILE_WRITER_H

#include <string>

namespace nux
{

/**
 * Write to a file asynchronously.
 *
 * This uses the GIO async functions, and as such depend on the gobject main
 * loop.
 */
class AsyncFileWriter
{
public:
  AsyncFileWriter(std::string const& filename);
  ~AsyncFileWriter();

  // Queue the data for writing.  It'll happen some time.
  void Write(std::string const& data);
  // Force all queued data to be written to disk.
  void Sync();

private:
  class Impl;
  Impl* pimpl;
};

}

#endif
