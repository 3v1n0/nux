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
#include "LoggerModules.h"

namespace nux {
namespace logging {

class Logger::Impl
{
public:
  Impl(std::string const& module)
    : module_(internal::LoggerModules::Instance().GetModule(module))
    {}

  std::string const& module() const
    {
      return module_.module();
    }

  bool IsErrorEnabled() const
    {
      return module_.IsErrorEnabled();
    }

  bool IsWarningEnabled() const
    {
      return module_.IsWarningEnabled();
    }

  bool IsInfoEnabled() const
    {
      return module_.IsInfoEnabled();
    }

  bool IsDebugEnabled() const
    {
      return module_.IsDebugEnabled();
    }

  void SetLogLevel(Level level)
    {
      module_.SetLogLevel(level);
    }

  Level GetLogLevel() const
    {
      return module_.GetLogLevel();
    }
  Level GetEffectiveLogLevel() const
    {
      return module_.GetEffectiveLogLevel();
    }

private:
  internal::LoggerModule& module_;
};


Logger::Logger(std::string const& component)
  : pimpl(new Impl(component))
{
}

Logger::~Logger()
{
  delete pimpl;
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


} // namespace logging
} // namespace nux
