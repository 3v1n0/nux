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
#ifndef NUX_CORE_LOGGING_WRITER_H
#define NUX_CORE_LOGGING_WRITER_H

#include <iosfwd>
#include <ctime>
#include <boost/utility.hpp>

#include "Logger.h"
/**
 * The Writer singleton is responsible for controlling where the
 * logging message go.  For testing purposes it is able to define the logging
 * stream, normally a std::stringstream.
 *
 * The default behaviour is to output to std::cout.
 *
 * As far as logging the timestamp goes, we only go to second precision in the
 * logging format itself.  If a high performance timer is needed it should be
 * managed by the caller.
 */

namespace nux {
namespace logging {

class Writer : boost::noncopyable
{
public:
  static Writer& Instance();
  ~Writer();

  void WriteMessage(Level severity,
                    std::string const& module,
                    std::string const& filename,
                    int line_number,
                    std::time_t timestamp,
                    std::string const& message);

  void SetOutputStream(std::ostream& out);

private:
  Writer();

private:
  class Impl;
  Impl* pimpl;
};

}
}

#endif
