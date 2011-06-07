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

#include "LoggingWriter.h"

#include <iostream>
#include <sstream>
#include <vector>

namespace nux {
namespace logging {

namespace {

struct StreamWrapper
{
  typedef boost::shared_ptr<StreamWrapper> Ptr;

  StreamWrapper(std::ostream& output) : out(output) {}
  std::ostream& out;
};

const char* severity_string(Level severity)
{
  // The return value is always padded out here.
  switch (severity)
  {
  case CRITICAL:
    return "CRIT ";
  case ERROR:
    return "ERROR";
  case WARNING:
    return "WARN ";
  case INFO:
    return "INFO ";
  case DEBUG:
    return "DEBUG";
  case TRACE:
    return "TRACE";
  default:
    // We technically shouldn't get this.
    return "     ";
  }
}

std::string timestamp_string(std::time_t timestamp)
{
  // return an ISO format string: YYYY-MM-DD HH:MM:SS
  static const char* format = "%Y-%m-%d %H:%M:%S";
  // NOTE: this is not tread-safe as localtime uses a static buffer.
  const int buf_size = 20;
  char buffer[buf_size];
  std::strftime(buffer, buf_size, format, std::localtime(&timestamp));
  return buffer;
}

} // anon namespace

class Writer::Impl
{
public:
  Impl();

  void WriteMessage(Level severity,
                    std::string const& module,
                    std::string const& filename,
                    int line_number,
                    std::time_t timestamp,
                    std::string const& message);

  void SetOutputStream(std::ostream& out);

private:
  typedef std::vector<StreamWrapper::Ptr> OutputStreams;
  OutputStreams output_streams_;
};

Writer::Impl::Impl()
{
  output_streams_.push_back(StreamWrapper::Ptr(new StreamWrapper(std::cerr)));
}

void Writer::Impl::SetOutputStream(std::ostream& out)
{
  output_streams_.clear();
  output_streams_.push_back(StreamWrapper::Ptr(new StreamWrapper(out)));
}

void Writer::Impl::WriteMessage(Level severity,
                                std::string const& module,
                                std::string const& filename,
                                int line_number,
                                std::time_t timestamp,
                                std::string const& message)
{
  // If we want to have some form of custom formatter, here is where we do it.
  // Right now, format the line independently, and then write it to each
  // output stream.
  std::stringstream sout;
  sout << severity_string(severity)
       << " " << timestamp_string(timestamp)
       << " " << module
       << " " << filename << ":" << line_number
       << " " << message;
  for (OutputStreams::iterator i = output_streams_.begin(), end = output_streams_.end();
       i != end; ++i)
  {
    std::ostream& out = (*i)->out;
    out << sout.rdbuf() << std::endl;
  }
}


Writer::Writer()
  : pimpl(new Impl())
{}

Writer::~Writer()
{
  delete pimpl;
}

Writer& Writer::Instance()
{
  static Writer instance;
  return instance;
}

void Writer::WriteMessage(Level severity,
                          std::string const& module,
                          std::string const& filename,
                          int line_number,
                          std::time_t timestamp,
                          std::string const& message)
{
  pimpl->WriteMessage(severity, module, filename, line_number,
                      timestamp, message);
}

void Writer::SetOutputStream(std::ostream& out)
{
  pimpl->SetOutputStream(out);
}

}
}
