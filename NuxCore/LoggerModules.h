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
#ifndef NUX_CORE_LOGGERMODULES_H
#define NUX_CORE_LOGGERMODULES_H

#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "Logger.h"

namespace nux {
namespace logging {
namespace internal {

class LoggerModule
{
public:
  typedef boost::shared_ptr<LoggerModule> Ptr;

  LoggerModule(std::string const& module, LoggerModule* parent);

  std::string const& module() const;

  bool IsErrorEnabled() const;
  bool IsWarningEnabled() const;
  bool IsInfoEnabled() const;
  bool IsDebugEnabled() const;

  void SetLogLevel(Level level);
  Level GetLogLevel() const;
  Level GetEffectiveLogLevel() const;

private:
  std::string module_;
  Level level_;
  LoggerModule* parent_;
};

class LoggerModules : boost::noncopyable
{
public:
  static LoggerModules& Instance();

  LoggerModule& GetModule(std::string const& module);

private:
  LoggerModules();

private:
  typedef std::map<std::string, LoggerModule::Ptr> ModuleMap;
  ModuleMap modules_;
  LoggerModule::Ptr root_;
};


inline std::string const& LoggerModule::module() const
{
  return module_;
}

inline bool LoggerModule::IsErrorEnabled() const
{
  return GetEffectiveLogLevel() <= ERROR;
}

inline bool LoggerModule::IsWarningEnabled() const
{
  return GetEffectiveLogLevel() <= WARNING;
}

inline bool LoggerModule::IsInfoEnabled() const
{
  return GetEffectiveLogLevel() <= INFO;
}

inline bool LoggerModule::IsDebugEnabled() const
{
  return GetEffectiveLogLevel() <= DEBUG;
}

inline void LoggerModule::SetLogLevel(Level level)
{
  // The root module can't be unspecified.
  if (module_ == "" && level == NOT_SPECIFIED)
    level = WARNING;
  level_ = level;
}

inline Level LoggerModule::GetLogLevel() const
{
  return level_;
}

inline Level LoggerModule::GetEffectiveLogLevel() const
{
  if (level_ == NOT_SPECIFIED && parent_)
    return parent_->GetEffectiveLogLevel();
  else
    return level_;
}

}
}
}

#endif
