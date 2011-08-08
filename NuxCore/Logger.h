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
#ifndef NUX_CORE_LOGGER_H
#define NUX_CORE_LOGGER_H

#include <ostream>
#include <string>
#include <boost/shared_ptr.hpp>

#if defined(NUX_OS_WINDOWS)
  #define __func__ __FUNCTION__
#endif

#define LOG_TRACE(logger) \
  if (!logger.IsTraceEnabled()) {} \
  else ::nux::logging::LogStream(::nux::logging::Trace, logger.module(), __FILE__, __LINE__).stream()
#define LOG_DEBUG(logger) \
  if (!logger.IsDebugEnabled()) {} \
  else ::nux::logging::LogStream(::nux::logging::Debug, logger.module(), __FILE__, __LINE__).stream()
#define LOG_INFO(logger) \
  if (!logger.IsInfoEnabled()) {} \
  else ::nux::logging::LogStream(::nux::logging::Info, logger.module(), __FILE__, __LINE__).stream()
#define LOG_WARN(logger) LOG_WARNING(logger)
#define LOG_WARNING(logger) \
  if (!logger.IsWarningEnabled()) {} \
  else ::nux::logging::LogStream(::nux::logging::Warning, logger.module(), __FILE__, __LINE__).stream()
#define LOG_ERROR(logger) \
  if (!logger.IsErrorEnabled()) {} \
  else ::nux::logging::LogStream(::nux::logging::Error, logger.module(), __FILE__, __LINE__).stream()

// We shouldn't really be logging block level information at anything higher
// than debug.
#define LOG_TRACE_BLOCK(logger) ::nux::logging::BlockTracer _block_tracer_ ## __LINE__ (logger, ::nux::logging::TRACE, __PRETTY_FUNC__, __FILE__, __LINE__)
#define LOG_DEBUG_BLOCK(logger) ::nux::logging::BlockTracer _block_tracer_ ## __LINE__ (logger, ::nux::logging::DEBUG, __PRETTY_FUNC__, __FILE__, __LINE__)


namespace nux {
namespace logging {

enum Level
{
  NotSpecified,
  Trace,
  Debug,
  Info,
  Warning,
  Error,
  Critical,
};

// Convert a string representation of a logging level into the enum value.
Level get_logging_level(std::string level);

/**
 * Configure multiple logging modules.
 *
 * This function expects a string of the format:
 *   module=info;sub.module=debug;other.module=warning
 *
 *
 * The root module can be specified by using the value "<root>", eg:
 *   <root>=info;other.module=debug
 *
 * The specified modules will have their logging level set to the specified
 * level as defined by the get_logging_level function.
 *
 * It is expected that this method is called during application startup with
 * the content of some environment variable.
 *   nux::logging::configure_logging(::getenv("MY_APP_LOGGING_CONFIG"));
 */
void configure_logging(const char* config_string);

std::string dump_logging_levels(std::string const& prefix = "");

class LogStream : public std::ostream
{
public:
  LogStream(Level severity,
            std::string const& module,
            std::string const& filename,
            int line_number);
  ~LogStream();

  std::ostream& stream() { return *this; }
};


class LoggerModule;
typedef boost::shared_ptr<LoggerModule> LoggerModulePtr;

class Logger
{
public:
  explicit Logger(std::string const& module);

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
  LoggerModulePtr pimpl;
};

/**
 * This class is used to log the entry and exit of a block.
 *
 * Entry is defined as where the object is created.  This is most likely going
 * to be defined using the macros defined above. Exit is defined as object
 * destruction, which is normally controlled through the end of scope killing
 * the stack object.
 *
 * int some_func(params...)
 * {
 *     LOG_TRACE_BLOCK(logger);
 *     ...
 * }
 */
class BlockTracer
{
public:
  BlockTracer(Logger& logger,
              Level level,
              std::string const& function_name,
              std::string const& filename,
              int line_number);
  ~BlockTracer();
private:
  Logger& logger_;
  Level level_;
  std::string function_name_;
  std::string filename_;
  int line_number_;
};

}
}


#endif
