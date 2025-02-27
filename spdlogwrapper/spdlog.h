#ifndef __SPDLOG_H__
#define __SPDLOG_H__

#include <memory>
#include <string>
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

namespace unix_ai
{
    class Logger
    {
    public:
        /// \brief      Get instance for spdlog
        /// \param      logger_name    log file's name
        /// \param      log_dir    the path of the log file to store
        /// \param      output_type    1: console, 2: file, 3: both
        /// \param      max_file_size    per logger file size
        /// \param      max_files    maximum number of files
        static Logger *getInstance(const std::string &logger_name = "default_logger",
                                   const std::string &log_dir = "/home/unix_ai/Work/log/tmp",
                                   int output_type = 1,
                                   const size_t &max_file_size = 10 * 1024 * 1024 /*10m*/,
                                   const size_t &max_files = 100)
        {
            static Logger Logger(logger_name, log_dir, output_type, max_file_size, max_files);
            return &Logger;
        }
        std::shared_ptr<spdlog::logger> getLogger();

    private:
        Logger(const std::string logger_name, const std::string log_dir, int output_type, const size_t &max_file_size, const size_t &max_files = 100);
        ~Logger();
        void *operator new(size_t size);
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        std::shared_ptr<spdlog::logger> m_logger;
    };

    // use embedded macro to support file and line number
#define LOG_TRACE(...) SPDLOG_LOGGER_CALL(unix_ai::Logger::getInstance()->getLogger().get(), spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_LOGGER_CALL(unix_ai::Logger::getInstance()->getLogger().get(), spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...) SPDLOG_LOGGER_CALL(unix_ai::Logger::getInstance()->getLogger().get(), spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...) SPDLOG_LOGGER_CALL(unix_ai::Logger::getInstance()->getLogger().get(), spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_CALL(unix_ai::Logger::getInstance()->getLogger().get(), spdlog::level::err, __VA_ARGS__)
}
#endif