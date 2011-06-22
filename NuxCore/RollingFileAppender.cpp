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
#include <boost/lexical_cast.hpp>

namespace bf = boost::filesystem;

namespace nux {
namespace logging {

namespace {

bf::path backup_path(bf::path path, unsigned number)
{
  if (number == 0)
    return path;

  std::string extension = path.extension();
  extension += "." + boost::lexical_cast<std::string>(number);
  return path.replace_extension(extension);
}

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
  // Firstly make sure that we have been given a full path.
  bf::path parent = filename.parent_path();
  if (parent.empty() || parent.root_directory().empty()) {
    std::string error_msg = filename.string() + " needs to be a full path";
    throw std::runtime_error(error_msg);
  }
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
  // If we aren't keeping backups, no rolling needed.
  if (number_of_backup_files_ == 0)
    return;

  unsigned backup = number_of_backup_files_;
  bf::path last_log(backup_path(filename_, backup));
  if (bf::exists(last_log)) {
    bf::remove(last_log);
  }
  // Move the previous files out.
  while (backup > 0) {
    bf::path prev_log(backup_path(filename_, --backup));
    if (bf::exists(prev_log)) {
      bf::rename(prev_log, last_log);
    }
    last_log = prev_log;
  }
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

} // anon namespace

RollingFileAppender::RollingFileAppender(std::string const& filename)
  : std::ostream(new RollingFileStreamBuffer(filename, 5, 1e7))
{
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
