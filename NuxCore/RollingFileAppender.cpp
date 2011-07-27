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

#include "NuxCore.h"
#include "RollingFileAppender.h"

#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>

#include <gio/gio.h>

#include <boost/lexical_cast.hpp>

#include "AsyncFileWriter.h"

namespace nux {
namespace logging {

namespace {

std::string backup_path(std::string const& path, unsigned number)
{
  if (number == 0)
    return path;

  std::ostringstream sout;
  sout << path << "." << number;
  return sout.str();
}

// Change the implementation to string buf
// Have a GFile representing the file on disk
// g_file_append gives a GFileOutputStream (subclass of GOutputStream)
// g_output_stream_write_async (only have one in progress at a time)
// g_output_stream_flush_async (probably don't want a pending async)
// keep our own count of the written bytes.
// Tests don't have a g_object main loop, so we have another way...
// while (g_main_context_pending(g_main_context_get_thread_default())) {
//   g_main_context_iteration(g_main_context_get_thread_default());
// }
class RollingFileStreamBuffer : public std::stringbuf
{
public:
  RollingFileStreamBuffer(std::string const& filename,
                          unsigned number_of_backup_files,
                          unsigned long long max_log_size,
                          sigc::signal<void>& files_rolled);
  ~RollingFileStreamBuffer();
protected:
  virtual int sync();
private:
  void AsyncWriterClosed();
  void RotateFiles();

#if defined(NUX_OS_WINDOWS) && (!defined(NUX_VISUAL_STUDIO_2010))
  std::tr1::shared_ptr<AsyncFileWriter> writer_;
#else
  std::shared_ptr<AsyncFileWriter> writer_;
#endif

  GFile* log_file_;
  std::string filename_;
  unsigned number_of_backup_files_;
  unsigned long long max_log_size_;
  unsigned long long bytes_written_;
  sigc::connection writer_closed_;
  sigc::signal<void>& files_rolled_;
};

RollingFileStreamBuffer::RollingFileStreamBuffer(std::string const& filename,
                                                 unsigned number_of_backup_files,
                                                 unsigned long long max_log_size,
                                                 sigc::signal<void>& files_rolled)
  : filename_(filename)
  , number_of_backup_files_(number_of_backup_files)
  , max_log_size_(max_log_size)
  , bytes_written_(0)
  , files_rolled_(files_rolled)
{
  // Make sure that the filename starts with a '/' for a full path.
  if (filename.empty() || filename[0] != '/')
  {
      std::string error_msg = "\"" + filename + "\" is not a full path";
      throw std::runtime_error(error_msg.c_str());
  }
  // Looks to see if our filename exists.
  if (g_file_test(filename.c_str(), G_FILE_TEST_EXISTS))
  {
    // The filename needs to be a regular file.
    if (!g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR))
    {
      std::string error_msg = filename + " is not a regular file";
      throw std::runtime_error(error_msg.c_str());
    }
    // Rotate the files.
    RotateFiles();
  }
  else
  {
    GFile* log_file = g_file_new_for_path(filename.c_str());
    GFile* log_dir = g_file_get_parent(log_file);
    if (log_dir)
    {
      g_file_make_directory_with_parents(log_dir, NULL, NULL);
      g_object_unref(log_dir);
      g_object_unref(log_file);
    }
    else
    {
      g_object_unref(log_file);
      std::string error_msg = "Can't get parent for " + filename;
      throw std::runtime_error(error_msg.c_str());
    }
  }
  // Now open the filename.
  writer_.reset(new AsyncFileWriter(filename));
  log_file_ = g_file_new_for_path(filename_.c_str());
}

RollingFileStreamBuffer::~RollingFileStreamBuffer()
{
  // We don't want notification when the writer closes now.
  if (writer_closed_.connected())
    writer_closed_.disconnect();
  g_object_unref(log_file_);
}

void RollingFileStreamBuffer::RotateFiles()
{
  // If we aren't keeping backups, no rolling needed.
  if (number_of_backup_files_ == 0)
    return;

  unsigned backup = number_of_backup_files_;
  std::string last_log(backup_path(filename_, backup));
  if (g_file_test(last_log.c_str(), G_FILE_TEST_EXISTS))
  {
    // Attempt to remove it.
    GFile* logfile = g_file_new_for_path(last_log.c_str());
    g_file_delete(logfile, NULL, NULL);
    g_object_unref(logfile);
  }
  // Move the previous files out.
  while (backup > 0)
  {
    std::string prev_log(backup_path(filename_, --backup));
    if (g_file_test(prev_log.c_str(), G_FILE_TEST_EXISTS))
    {
      GFile* dest = g_file_new_for_path(last_log.c_str());
      GFile* src = g_file_new_for_path(prev_log.c_str());
      // We don't really care if there are errors for now.
      g_file_move(src, dest, G_FILE_COPY_NONE, NULL, NULL, NULL, NULL);
      g_object_unref(src);
      g_object_unref(dest);
    }
    last_log = prev_log;
  }
}

void RollingFileStreamBuffer::AsyncWriterClosed()
{
  // Rotate the files and open a new file writer.
  RotateFiles();
  writer_.reset(new AsyncFileWriter(filename_));
  bytes_written_ = 0;
  // We emit the files_rolled_ here and not in the RotateFiles method as the
  // RotateFiles is called from the constructor, which has a reference to the
  // files_rolled signal from the parent stream.  If this is emitted due
  // rotating the files in the contructor, we get a seg fault due to trying to
  // use the signal before it is constructed.
  files_rolled_.emit();
}

int RollingFileStreamBuffer::sync()
{
  // If the async file writer is in the middle of closing, there is nothing we can do.
  if (writer_->IsClosing())
    return 0;

  std::string message = str();
  // reset the stream
  str("");

  std::size_t message_size = message.size();
  if (message_size > 0)
  {
    bytes_written_ += message_size;
    writer_->Write(message);
    if (bytes_written_ > max_log_size_)
    {
      // Close the writer and once it is closed, rotate the files and open a new file.
      writer_closed_ = writer_->closed.connect(
        sigc::mem_fun(this, &RollingFileStreamBuffer::AsyncWriterClosed));
      writer_->Close();
    }
  }
  return 0; // success
}

} // anon namespace

RollingFileAppender::RollingFileAppender(std::string const& filename)
  : std::ostream(new RollingFileStreamBuffer(filename, 5, 1e7, files_rolled))
{
}

RollingFileAppender::RollingFileAppender(std::string const& filename,
                                         unsigned number_of_backup_files,
                                         unsigned long long max_log_size)
  : std::ostream(new RollingFileStreamBuffer(filename,
                                             number_of_backup_files,
                                             max_log_size,
                                             files_rolled))
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
