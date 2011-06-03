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
  Impl(std::string const& component);

  bool IsErrorEnabled() const;
  bool IsWarningEnabled() const;
  bool IsInfoEnabled() const;
  bool IsDebugEnabled() const;

private:
  std::string component_;

};


Logger::Impl::Impl(std::string const& component)
  : component_(component)
{

}

bool Logger::Impl::IsErrorEnabled() const
{
  return true;
}

bool Logger::Impl::IsWarningEnabled() const
{
  return true;
}

bool Logger::Impl::IsInfoEnabled() const
{
  return false;
}

bool Logger::Impl::IsDebugEnabled() const
{
  return false;
}



Logger::Logger(std::string const& component)
  : pimpl(new Impl(component))
{
}

Logger::~Logger()
{
  delete pimpl;
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


} // namespace logging
} // namespace nux
