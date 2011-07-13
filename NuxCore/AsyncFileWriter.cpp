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

#include <sstream>

#include <gio/gio.h>

#include <iostream>

namespace nux
{

/**
 * CAUTION: right now this class is not thread aware.  It assumes that all the
 * write calls are coming from a single thread.  Perhaps we need to fix this?
 */
class AsyncFileWriter::Impl
{
public:
  Impl(sigc::signal<void>& closed, std::string const& filename);
  ~Impl();

  void Write(std::string const& data);
  void Close();

  void ProcessAsync();

  static void AppendAsyncCallback(GFile* source, GAsyncResult* res, Impl* impl);
  static void WriteAsyncCallback(GOutputStream* source, GAsyncResult* res, Impl* impl);
  static void CloseAsyncCallback(GOutputStream* source, GAsyncResult* res, Impl* impl);

private:
  sigc::signal<void>& closed_;
  GCancellable* cancel_;
  GFile* file_;
  GFileOutputStream* output_stream_;
  bool close_pending_;
  bool pending_async_call_;

  std::stringstream pending_content_;
};


AsyncFileWriter::Impl::Impl(sigc::signal<void>& closed, std::string const& filename)
  : closed_(closed)
  , cancel_(g_cancellable_new())
  , file_(g_file_new_for_path(filename.c_str()))
  , output_stream_(0)
  , close_pending_(false)
  , pending_async_call_(true)
{
  g_file_append_to_async(file_,
                         G_FILE_CREATE_NONE,
                         G_PRIORITY_DEFAULT,
                         cancel_,
                         (GAsyncReadyCallback)&AsyncFileWriter::Impl::AppendAsyncCallback,
                         this);
}

AsyncFileWriter::Impl::~Impl()
{
  if (pending_async_call_)
  {
    std::cerr << "Cancelling the pending async call\n";
    g_cancellable_cancel(cancel_);
  }
  // make sure the file is closed.
  if (output_stream_)
  {
    // If we had an output stream, sync write any pending content.
    if (pending_content_.tellp())
    {
      std::cerr << "Writing pending content\n";
      std::string data(pending_content_.str());
      gsize bytes_written;
      g_output_stream_write_all((GOutputStream*)output_stream_,
                                data.c_str(),
                                data.size(),
                                &bytes_written,
                                NULL, NULL);
    }
    closed_.emit();
    g_object_unref(output_stream_);
  }

  g_object_unref(file_);
  g_object_unref(cancel_);
}

void AsyncFileWriter::Impl::AppendAsyncCallback(GFile* source,
                                                GAsyncResult* res,
                                                Impl* impl)
{
  std::cerr << "Append Async Callback: " << impl << "\n";
  impl->pending_async_call_ = false;
  impl->output_stream_ = g_file_append_to_finish(source, res, NULL);
  impl->ProcessAsync();
}

void AsyncFileWriter::Impl::Write(std::string const& data)
{
  if (close_pending_) return;
  // TODO: lock the pending_content_ access
  pending_content_ << data;
  ProcessAsync();
}

void AsyncFileWriter::Impl::ProcessAsync()
{
  if (output_stream_ == NULL || pending_async_call_) return;

  if (pending_content_.tellp())
  {
    // TODO: lock the pending_content_ access
    std::string data = pending_content_.str();
    g_output_stream_write_async((GOutputStream*)output_stream_,
                                data.c_str(),
                                data.size(),
                                G_PRIORITY_DEFAULT,
                                cancel_,
                                (GAsyncReadyCallback)&AsyncFileWriter::Impl::WriteAsyncCallback,
                                this);
    pending_async_call_ = true;
  }
  else if (close_pending_)
  {
    g_output_stream_close_async((GOutputStream*)output_stream_,
                                G_PRIORITY_DEFAULT,
                                cancel_,
                                (GAsyncReadyCallback)&AsyncFileWriter::Impl::CloseAsyncCallback,
                                this);
    pending_async_call_ = true;
  }
}

void AsyncFileWriter::Impl::WriteAsyncCallback(GOutputStream* source,
                                               GAsyncResult* res,
                                               Impl* impl)
{
  impl->pending_async_call_ = false;
  std::size_t bytes_written = g_output_stream_write_finish(source, res, NULL);
  // TODO: lock the pending_content_ access
  std::string data = impl->pending_content_.str();
  if (bytes_written >= data.size()) {
    // There should be no reason why bytes_written should be greater than the
    // number of bytes in the stream, but this is paranoia.
    impl->pending_content_.str("");
  } else {
    impl->pending_content_.str(data.substr(bytes_written));
  }
  impl->ProcessAsync();
}

void AsyncFileWriter::Impl::Close()
{
  close_pending_ = true;
  ProcessAsync();
}

void AsyncFileWriter::Impl::CloseAsyncCallback(GOutputStream* source,
                                               GAsyncResult* res,
                                               Impl* impl)
{
  // No point actually checking to see if the file was closed properly as
  // there is no other action we can take on it.
  g_object_unref(impl->output_stream_);
  impl->output_stream_ = 0;
  impl->closed_.emit();
}

AsyncFileWriter::AsyncFileWriter(std::string const& filename)
  : pimpl(new Impl(closed, filename))
{}

AsyncFileWriter::~AsyncFileWriter()
{
  delete pimpl;
}

void AsyncFileWriter::Write(std::string const& data)
{
  pimpl->Write(data);
}

void AsyncFileWriter::Close()
{
  pimpl->Close();
}


} // namespace nux
