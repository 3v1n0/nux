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

#include <memory>
#include <ostream>
#include <string>

#if defined(NUX_OS_WINDOWS)
  #define __func__ __FUNCTION__
#endif

#define LOG_TRACE(logger) \
  if (!Unwrap(logger).IsTraceEnabled()) {}                              \
  else ::nux::logging::LogStream(::nux::logging::Trace, Unwrap(logger).module(), __FILE__, __LINE__).stream()
#define LOG_DEBUG(logger) \
  if (!Unwrap(logger).IsDebugEnabled()) {} \
  else ::nux::logging::LogStream(::nux::logging::Debug, Unwrap(logger).module(), __FILE__, __LINE__).stream()
#define LOG_INFO(logger) \
  if (!Unwrap(logger).IsInfoEnabled()) {} \
  else ::nux::logging::LogStream(::nux::logging::Info, Unwrap(logger).module(), __FILE__, __LINE__).stream()
#define LOG_WARN(logger) LOG_WARNING(logger)
#define LOG_WARNING(logger) \
  if (!Unwrap(logger).IsWarningEnabled()) {} \
  else ::nux::logging::LogStream(::nux::logging::Warning, Unwrap(logger).module(), __FILE__, __LINE__).stream()
#define LOG_ERROR(logger) \
  if (!Unwrap(logger).IsErrorEnabled()) {} \
  else ::nux::logging::LogStream(::nux::logging::Error, Unwrap(logger).module(), __FILE__, __LINE__).stream()

// We shouldn't really be logging block level information at anything higher than debug.
#if defined(NUX_OS_WINDOWS)
  #define LOG_TRACE_BLOCK(logger)
  #define LOG_DEBUG_BLOCK(logger)
#else
  #define LOG_TRACE_BLOCK(logger) ::nux::logging::BlockTracer _block_tracer_ ## __LINE__ (Unwrap(logger), ::nux::logging::Trace, __PRETTY_FUNCTION__, __FILE__, __LINE__)
  #define LOG_DEBUG_BLOCK(logger) ::nux::logging::BlockTracer _block_tracer_ ## __LINE__ (Unwrap(logger), ::nux::logging::Debug, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#endif

/*
 * This macro is used to declare a file scoped logger module function.  The
 * purpose of this is to allow a file scoped logger that doesn't use static
 * initialisation, but instead is a method that creates the object the first
 * time it is used.
 *
 * Used like:
 *
 * DECLARE_LOGGER(logger, "test.module");
 *
 * Then used like this in some function (just like a normal logger object):
 *
 * LOG_DEBUG(logger) << "foo";
 */
#define DECLARE_LOGGER(logger, module)               \
namespace {                                          \
  ::nux::logging::Logger& logger()                   \
  {                                                  \
    static ::nux::logging::Logger instance(module);  \
    return instance;                                 \
  }                                                  \
}


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
std::string Backtrace(int levels = -1);

void reset_logging();
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
typedef std::shared_ptr<LoggerModule> LoggerModulePtr;

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


// The Unwrap function is used by the LOG_* macros above to accept either a
// reference to a Logger object, or a function that when called returns a
// reference to a Logger object.
inline Logger const& Unwrap(Logger const& logger)
{
  return logger;
}

typedef Logger& (*LoggerFunc)();
inline Logger const& Unwrap(LoggerFunc func)
{
  return func();
}



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
  BlockTracer(Logger const& logger,
              Level level,
              std::string const& function_name,
              std::string const& filename,
              int line_number);
  ~BlockTracer();
private:
  Logger const& logger_;
  Level level_;
  std::string function_name_;
  std::string filename_;
  int line_number_;
};

}
}


#endif
