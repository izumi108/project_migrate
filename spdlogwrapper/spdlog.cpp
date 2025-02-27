#include "spdlog.h"
#include <time.h>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace unix_ai;

static inline int NowDateToInt()
{
    time_t now;
    time(&now);

    // choose thread safe version in each platform
    tm p;
#ifdef _WIN32
    localtime_s(&p, &now);
#else
    localtime_r(&now, &p);
#endif // _WIN32

    int now_date = (1900 + p.tm_year) * 10000 + (p.tm_mon + 1) * 100 + p.tm_mday;
    return now_date;
}

static inline std::string NowTimeToInt()
{
    time_t now;
    time(&now);

    // choose thread safe version in each platform
    tm p;
#ifdef _WIN32
    localtime_s(&p, &now);
#else
    localtime_r(&now, &p);
#endif // _WIN32

    int now_int = p.tm_hour * 10000 + p.tm_min * 100 + p.tm_sec;
    std::ostringstream oss;
    oss << std::setw(6) << std::setfill('0') << now_int;
    return oss.str();
}

std::shared_ptr<spdlog::logger> Logger::getLogger()
{
    return m_logger;
}

Logger::Logger(const std::string logger_name, const std::string log_dir, int output_type, const size_t &max_file_size, const size_t &max_files)
{
    try
    {
        // Logger name with timestamp
        int date = NowDateToInt();
        const std::string logger_full_name = logger_name + "_" + std::to_string(date) + "_" + NowTimeToInt();

        // Create sinks based on output_type
        std::vector<spdlog::sink_ptr> sinks;

        if (output_type == 1 || output_type == 3) // Console or Both
        {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::trace);
            sinks.push_back(console_sink);
        }

        if (output_type == 2 || output_type == 3) // File or Both
        {
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_dir + "/" + logger_full_name + ".log", max_file_size, max_files);
            file_sink->set_level(spdlog::level::trace);
            sinks.push_back(file_sink);
        }

        m_logger = std::make_shared<spdlog::logger>(logger_name, sinks.begin(), sinks.end());
        m_logger->set_level(spdlog::level::trace);
        m_logger->flush_on(spdlog::level::trace);

        // Set pattern
        m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");

        spdlog::register_logger(m_logger);
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    }
}

Logger::~Logger()
{
    spdlog::drop_all();
}

void *Logger::operator new(size_t size)
{
}