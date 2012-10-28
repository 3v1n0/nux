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
#ifndef NUX_CORE_LOGGER_PRIVATE_H
#define NUX_CORE_LOGGER_PRIVATE_H

#include <string>
#include <map>
#include <boost/utility.hpp>

#include "Logger.h"
#include "LoggingWriter.h"


namespace nux {
namespace logging {


class LoggerModule
{
public:
  LoggerModule(std::string const& module, LoggerModulePtr const& parent);

  std::string const& module() const;

  bool IsErrorEnabled() const;
  bool IsWarningEnabled() const;
  bool IsInfoEnabled() const;
  bool IsDebugEnabled() const;
  bool IsTraceEnabled() const;

  void SetLogLevel(Level level);
  Level GetLogLevel() const;
  Level GetEffectiveLogLevel() const;

private:
  std::string module_;
  Level level_;
  LoggerModulePtr parent_;
  // An attempt to make sure the writer is around for as long as the loggers.
  Writer& writer_;
};

class LoggerModules : boost::noncopyable
{
public:
  static LoggerModules& Instance();

  LoggerModulePtr const& GetModule(std::string const& module);

  void Reset();
  std::string DumpLoggingLevels(std::string const& prefix) const;

  bool HasModule(std::string const& module) const;

private:
  LoggerModules();

private:
  typedef std::map<std::string, LoggerModulePtr> ModuleMap;
  ModuleMap modules_;
  LoggerModulePtr root_;
};


}
}


#endif
