#ifndef __LOG_WRAPPER_H__
#define __LOG_WRAPPER_H__
#define SPDLOG_HEADER_ONLY

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "nlohmann/json.hpp"
using namespace nlohmann;

#include "spdlog/async.h"
#include "spdlog/async_logger.h"
#include "spdlog/details/thread_pool-inl.h"
#include "spdlog/details/thread_pool.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#define LOG_OUTPUT_FORMAT "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v"
#define LOG_TRACE_L(loggerName, ...) \
  SPDLOG_LOGGER_CALL(spdlog::get(loggerName), spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG_L(loggerName, ...) \
  SPDLOG_LOGGER_CALL(spdlog::get(loggerName), spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO_L(loggerName, ...) \
  SPDLOG_LOGGER_CALL(spdlog::get(loggerName), spdlog::level::info, __VA_ARGS__)
#define LOG_WARN_L(loggerName, ...) \
  SPDLOG_LOGGER_CALL(spdlog::get(loggerName), spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR_L(loggerName, ...) \
  SPDLOG_LOGGER_CALL(spdlog::get(loggerName), spdlog::level::err, __VA_ARGS__)
#define LOG_CRITI_L(loggerName, ...)                                   \
  SPDLOG_LOGGER_CALL(spdlog::get(loggerName), spdlog::level::critical, \
                     __VA_ARGS__)
#ifndef MULTIMODULE
#define loggerName "log"
#define LOG_TRACE(...) LOG_TRACE_L(loggerName, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_DEBUG_L(loggerName, __VA_ARGS__)
#define LOG_INFO(...) LOG_INFO_L(loggerName, __VA_ARGS__)
#define LOG_WARN(...) LOG_WARN_L(loggerName, __VA_ARGS__)
#define LOG_ERROR(...) LOG_ERROR_L(loggerName, __VA_ARGS__)
#define LOG_CRITI(...) LOG_CRITI_L(loggerName, __VA_ARGS__)
#else
#define LOG_TRACE(loggerName, ...) LOG_TRACE_L(loggerName, __VA_ARGS__)
#define LOG_DEBUG(loggerName, ...) LOG_DEBUG_L(loggerName, __VA_ARGS__)
#define LOG_INFO(loggerName, ...) LOG_INFO_L(loggerName, __VA_ARGS__)
#define LOG_WARN(loggerName, ...) LOG_WARN_L(loggerName, __VA_ARGS__)
#define LOG_ERROR(loggerName, ...) LOG_ERROR_L(loggerName, __VA_ARGS__)
#define LOG_CRITI(loggerName, ...) LOG_CRITI_L(loggerName, __VA_ARGS__)
#endif

class LogWrapper {
  friend class Config;

 public:
  // Log output mode
  enum OutMode {
    SYNC = 0,
    ASYNC = 1,
  };
  static OutMode GetOutModeEnum(const std::string& strMode);

  // Log output level
  enum OutLevel {
    LEVEL_TRACE = 0,
    LEVEL_DEBUG = 1,
    LEVEL_INFO = 2,
    LEVEL_WARN = 3,
    LEVEL_ERROR = 4,
    LEVEL_CRITI = 5,
    LEVEL_OFF = 6,
  };
  static OutLevel GetOutLevelEnum(const std::string& strLevel);

 public:
  LogWrapper();
  ~LogWrapper();
  bool AddColorConsole(const char* pLoggerName = "log",
                       const OutLevel level = LEVEL_TRACE);
  bool AddRotatingFileCofig(const char* pFileName,
                            const char* pLoggerName = "log",
                            const int nMaxFileSize = 1024 * 1024 * 10,
                            const int nMaxFile = 100,
                            const OutLevel level = LEVEL_TRACE);
  bool AddRotatingFile(const char* pFileName, const char* logDir,
                       const char* pLoggerName = "log",
                       const int nMaxFileSize = 1024 * 10,
                       const int nMaxFile = 10,
                       const OutLevel level = LEVEL_TRACE);
  bool AddDailyFile(const char* pLoggerName, const char* pFileName,
                    const int nHour, const int nMinute,
                    const OutLevel eLevel = LEVEL_TRACE);
  // Initialize from json files
  bool InitConfig(const char* configFile);
  bool Init(const OutMode outMode = SYNC,
            const std::string strLogFormat = LOG_OUTPUT_FORMAT);
  void UnInit();

 private:
  std::vector<std::string> StringSplit(const std::string& strSrc,
                                       const std::string& strSplit);
  void UpdateSinkMap(std::string strLoggerName, spdlog::sink_ptr pSink);
  std::string GenerateLogFileName(const std::string& fileName,
                                  const std::string& logDir);
  void UpdateThreadPoolMap(
      std::string strLoggerName,
      std::shared_ptr<spdlog::details::thread_pool> pThreadPool);
  bool m_bInit;
  std::map<std::string, std::vector<spdlog::sink_ptr>> m_mapLoggerParam;
  std::map<std::string, std::shared_ptr<spdlog::details::thread_pool>>
      m_mapAsyncThreadPool;
};

template <class T>
T GetJsonValue(const std::string& strKey, const json& j) {
  try {
    if (j.contains(strKey)) {
      T t;
      t = j[strKey];
      return t;
    }
    return T();
  } catch (json::exception& e) {
    printf("GetJsonValue() failed! err:%s\n", e.what());
    return T();
  }
}
#endif
