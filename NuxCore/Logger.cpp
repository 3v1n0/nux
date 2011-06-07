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

#include "Logger.h"
#include "LoggingWriter.h"

#include <map>
#include <sstream>
#include <boost/utility.hpp>

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

  void SetLogLevel(Level level);
  Level GetLogLevel() const;
  Level GetEffectiveLogLevel() const;

private:
  std::string module_;
  Level level_;
  LoggerModulePtr parent_;
};

class LoggerModules : boost::noncopyable
{
public:
  static LoggerModules& Instance();

  LoggerModulePtr const& GetModule(std::string const& module);

private:
  LoggerModules();

private:
  typedef std::map<std::string, LoggerModulePtr> ModuleMap;
  ModuleMap modules_;
  LoggerModulePtr root_;
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


Logger::Logger(std::string const& module)
  : pimpl(LoggerModules::Instance().GetModule(module))
{
}

std::string const& Logger::module() const
{
  return pimpl->module();
}

bool Logger::IsErrorEnabled() const
{
  return pimpl->IsErrorEnabled();
}

bool Logger::IsWarningEnabled() const
{
  return pimpl->IsWarningEnabled();
}

bool Logger::IsInfoEnabled() const
{
  return pimpl->IsInfoEnabled();
}

bool Logger::IsDebugEnabled() const
{
  return pimpl->IsDebugEnabled();
}

void Logger::SetLogLevel(Level level)
{
  pimpl->SetLogLevel(level);
}

Level Logger::GetLogLevel() const
{
  return pimpl->GetLogLevel();
}

Level Logger::GetEffectiveLogLevel() const
{
  return pimpl->GetEffectiveLogLevel();
}


LoggerModule::LoggerModule(std::string const& module,
                           LoggerModulePtr const& parent)
  : module_(module)
  , level_(NOT_SPECIFIED)
  , parent_(parent)
{
}

LoggerModules::LoggerModules()
  : root_(new LoggerModule("", LoggerModulePtr()))
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

LoggerModulePtr const& LoggerModules::GetModule(std::string const& module)
{
  ModuleMap::iterator i = modules_.find(module);
  if (i != modules_.end())
    return i->second;

  // Make the new LoggerModule and its parents.
  // Split on '.'
  std::string::size_type idx = module.rfind(".");
  LoggerModulePtr parent = root_;
  if (idx != std::string::npos) {
    parent = GetModule(module.substr(0, idx));
  }
  LoggerModulePtr logger(new LoggerModule(module, parent));
  // std::map insert method returns a pair<iterator, bool> which seems
  // overly annoying to make a temporary of, so just return the const
  // reference pointed to by the interator.
  return modules_.insert(ModuleMap::value_type(module, logger)).first->second;
}


class LogStreamBuffer : public std::stringbuf
{
public:
  LogStreamBuffer(Level severity,
                  std::string const& module,
                  std::string const& filename,
                  int line_number);
protected:
  virtual int sync();
private:
  Level severity_;
  std::string module_;
  std::string filename_;
  int line_number_;
  std::time_t timestamp_;
};

LogStream::LogStream(Level severity,
                     std::string const& module,
                     std::string const& filename,
                     int line_number)
  : std::ostream(0)
{
  // Set the buffer with a dynamically created LogStreamBuffer.
  LogStreamBuffer* buff = new LogStreamBuffer(severity, module,
                                              filename, line_number);
  rdbuf(buff);
}

LogStream::~LogStream()
{
  rdbuf()->pubsync();
  std::streambuf* buff = rdbuf(0);
  delete buff;
}


LogStreamBuffer::LogStreamBuffer(Level severity,
                                 std::string const& module,
                                 std::string const& filename,
                                 int line_number)
  : std::stringbuf(std::ios_base::out)
  , severity_(severity)
  , module_(module)
  , filename_(filename)
  , line_number_(line_number)
  , timestamp_(std::time(0))
{
}

int LogStreamBuffer::sync()
{
  std::string message = str();
  // reset the stream
  str("");
  // Only log the message if there is something there.
  if (!message.empty())
    Writer::Instance().WriteMessage(severity_, module_,
                                    filename_, line_number_,
                                    timestamp_, message);
  return 0; // success
}

} // namespace logging
} // namespace nux
