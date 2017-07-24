/**
 * @file logger.h
 * @brief Logging macros that can be optimized out
 * @author Hideaki, modified by Anuj
 */

#ifndef ERPC_LOGGER_H
#define ERPC_LOGGER_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>
#include <string>

namespace ERpc {

// Log levels: higher means more verbose
#define LOG_LEVEL_OFF 0
#define LOG_LEVEL_ERROR 1  // Only fatal conditions
#define LOG_LEVEL_WARN 2   // Conditions from which it is possible to recover
#define LOG_LEVEL_INFO 3   // Reasonable to print (e.g., management packets)
#define LOG_LEVEL_DEBUG 4  // Too frequent to print (e.g., reordered packets)
#define LOG_LEVEL_TRACE 5  // Extremely frequent (e.g., all datapath packets)

#define LOG_OUTPUT_STREAM stdout

// If LOG_LEVEL is not defined, default to LOG_LEVEL_INFO in debug mode, and
// LOG_LEVEL_WARN in non-debug mode.
#ifndef LOG_LEVEL
#ifndef NDEBUG
#define LOG_LEVEL LOG_LEVEL_INFO
#else
#define LOG_LEVEL LOG_LEVEL_WARN
#endif
#endif

void output_log_header(int level);

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(...)                     \
  output_log_header(LOG_LEVEL_ERROR);      \
  fprintf(LOG_OUTPUT_STREAM, __VA_ARGS__); \
  fflush(stdout)
#else
#define LOG_ERROR(...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(...)                      \
  output_log_header(LOG_LEVEL_WARN);       \
  fprintf(LOG_OUTPUT_STREAM, __VA_ARGS__); \
  fflush(stdout)
#else
#define LOG_WARN(...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(...)                      \
  output_log_header(LOG_LEVEL_INFO);       \
  fprintf(LOG_OUTPUT_STREAM, __VA_ARGS__); \
  fflush(stdout)
#else
#define LOG_INFO(...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_DEBUG(...)                     \
  output_log_header(LOG_LEVEL_DEBUG);      \
  fprintf(LOG_OUTPUT_STREAM, __VA_ARGS__); \
  fflush(stdout)
#else
#define LOG_DEBUG(...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_TRACE
#define LOG_TRACE(...)                     \
  output_log_header(LOG_LEVEL_TRACE);      \
  fprintf(LOG_OUTPUT_STREAM, __VA_ARGS__); \
  fflush(stdout)
#else
#define LOG_TRACE(...) ((void)0)
#endif

// Output log message header
inline void output_log_header(int level) {
  const boost::posix_time::ptime now =
      boost::posix_time::microsec_clock::local_time();
  const boost::posix_time::time_duration td = now.time_of_day();

  const long minutes = td.minutes();
  const long seconds = td.seconds();
  const long milliseconds =
      td.total_milliseconds() -
      ((td.hours() * 3600 + minutes * 60 + seconds) * 1000);

  const char *type;
  switch (level) {
    case LOG_LEVEL_ERROR:
      type = "ERROR";
      break;
    case LOG_LEVEL_WARN:
      type = "WARN";
      break;
    case LOG_LEVEL_INFO:
      type = "INFO";
      break;
    case LOG_LEVEL_DEBUG:
      type = "DEBUG";
      break;
    case LOG_LEVEL_TRACE:
      type = "TRACE";
      break;
    default:
      type = "UNKWN";
  }

  fprintf(LOG_OUTPUT_STREAM, "%02ld:%02ld.%03ld %s: ", minutes, seconds,
          milliseconds, type);
}

/// Return true iff DEBUG and TRACE mode logging is disabled. These modes can
/// print an unreasonable number of log messages.
static bool is_log_level_reasonable() { return LOG_LEVEL <= LOG_LEVEL_INFO; }

}  // End ERpc

#endif  // ERPC_LOGGER_H