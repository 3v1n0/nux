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

#include "RollingFileAppender.h"

#include <fstream>
#include <stdexcept>
#include <boost/filesystem.hpp>

namespace bf = boost::filesystem;

namespace nux {
namespace logging {

class RollingFileStreamBuffer : public std::basic_filebuf<char>
{
public:
  typedef std::basic_filebuf<char> BaseFileBuf;

  RollingFileStreamBuffer(bf::path const& filename,
                          unsigned number_of_backup_files,
                          unsigned long long max_log_size);
protected:
  virtual int sync();
private:
  void RotateFiles();

  bf::path filename_;
  unsigned number_of_backup_files_;
  unsigned long long max_log_size_;
};

RollingFileStreamBuffer::RollingFileStreamBuffer(bf::path const& filename,
                                                 unsigned number_of_backup_files,
                                                 unsigned long long max_log_size)
  : filename_(filename)
  , number_of_backup_files_(number_of_backup_files)
  , max_log_size_(max_log_size)
{
  // Looks to see if our filename exists.
  if (bf::exists(filename)) {
    // The filename needs to be a regular file.
    if (!bf::is_regular_file(filename)) {
      std::string error_msg = filename.string() + " is not a regular file";
      throw std::runtime_error(error_msg.c_str());
    }
    // Rotate the files.
    RotateFiles();
  } else {
    // Firstly make sure that the directory for the filename exists.
    bf::path parent = filename.parent_path();
    if (parent.empty()) {
      std::string error_msg = filename.string() + " needs to be a full path";
      throw std::runtime_error(error_msg);
    }
    bf::create_directories(parent);
  }
  // Now open the filename.
  BaseFileBuf* buff = open(filename.string().c_str(), std::ios_base::out);
  if (!buff) {
    std::string error_msg = "Failed to open " + filename.string() + " for writing";
    throw std::runtime_error(error_msg);
  }
}

void RollingFileStreamBuffer::RotateFiles()
{
}

int RollingFileStreamBuffer::sync()
{
  BaseFileBuf::sync();
  // Look at the size of the file, and if it is bigger than max_log_size then
  // we rotate the files.
  if (bf::file_size(filename_) > max_log_size_) {
    close();
    RotateFiles();
    open(filename_.string().c_str(), std::ios_base::out);
  }
  return 0; // success
}


RollingFileAppender::RollingFileAppender(std::string const& filename,
                                         unsigned number_of_backup_files,
                                         unsigned long long max_log_size)
  : std::ostream(new RollingFileStreamBuffer(filename,
                                             number_of_backup_files,
                                             max_log_size))
{
}

RollingFileAppender::~RollingFileAppender()
{
  rdbuf()->pubsync();
  std::streambuf* buff = rdbuf(0);
  delete buff;
}


} // namespace logging
} // namespace nux
