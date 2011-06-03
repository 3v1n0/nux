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


namespace nux {
namespace logging {

class Logger::Impl
{
public:
  Impl(std::string const& module);

  std::string const& module() const
    {
      return module_;
    }

  bool IsErrorEnabled() const;
  bool IsWarningEnabled() const;
  bool IsInfoEnabled() const;
  bool IsDebugEnabled() const;

  void SetLogLevel(Level level);

private:
  std::string module_;
  Level level_;
};


Logger::Impl::Impl(std::string const& module)
  : module_(module)
  , level_(WARNING)
{

}

bool Logger::Impl::IsErrorEnabled() const
{
  return level_ <= ERROR;
}

bool Logger::Impl::IsWarningEnabled() const
{
  return level_ <= WARNING;
}

bool Logger::Impl::IsInfoEnabled() const
{
  return level_ <= INFO;
}

bool Logger::Impl::IsDebugEnabled() const
{
  return level_ <= DEBUG;
}

void Logger::Impl::SetLogLevel(Level level)
{
  level_ = level;
}



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

} // namespace logging
} // namespace nux
