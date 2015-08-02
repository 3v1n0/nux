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

#include "Helpers.h"

#include "NuxCore/LoggingWriter.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace nux
{
namespace testing
{

std::string ReadFile(std::string const& filename)
{
  std::ifstream input(filename.c_str());
  if (input.bad())
    throw std::runtime_error("bad file");
  return std::string((std::istreambuf_iterator<char>(input)),
                     std::istreambuf_iterator<char>());
}

void PumpGObjectMainLoop()
{
  GMainContext* context(g_main_context_get_thread_default());
  while (g_main_context_pending(context)) {
    g_main_context_iteration(context, false);
  }
}

CaptureLogOutput::CaptureLogOutput()
{
  logging::Writer::Instance().SetOutputStream(sout_);
}

CaptureLogOutput::~CaptureLogOutput()
{
  logging::Writer::Instance().SetOutputStream(std::cout);
}

std::string CaptureLogOutput::GetOutput()
{
  std::string result = sout_.str();
  sout_.str("");
  return result;
}


}
}
