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

#include "LoggerModules.h"

namespace nux {
namespace logging {
namespace internal {

LoggerModule::LoggerModule(std::string const& module, LoggerModule* parent)
  : module_(module)
  , level_(NOT_SPECIFIED)
  , parent_(parent)
{
}

LoggerModules::LoggerModules()
  : root_(new LoggerModule("", 0))
{
  // Make sure we have the root logger available.
  root_->SetLogLevel(WARNING);
  modules_.insert(ModuleMap::value_type("", root_));
}

LoggerModules& LoggerModules::Instance()
{
  static LoggerModules instance;
  return instance;
}

LoggerModule& LoggerModules::GetModule(std::string const& module)
{
  ModuleMap::iterator i = modules_.find(module);
  if (i != modules_.end())
    return *(i->second);

  // Make the new LoggerModule and its parents.
  // Split on '.'
  std::string::size_type idx = module.rfind(".");
  LoggerModule* parent = root_.get();
  if (idx != std::string::npos) {
    parent = &GetModule(module.substr(0, idx));
  }
  LoggerModule::Ptr logger(new LoggerModule(module, parent));
  modules_.insert(ModuleMap::value_type(module, logger));
  return *logger;
}



} // namespace internal
} // namespace logging
} // namespace nux
