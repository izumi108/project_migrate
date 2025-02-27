#include "log_wrapper.h"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
struct FileInfo {
  std::string path;
  time_t modification_time;
};
static inline int NowDateToInt() {
  time_t now;
  time(&now);

  // choose thread safe version in each platform
  tm p;
#ifdef _WIN32
  localtime_s(&p, &now);
#else
  localtime_r(&now, &p);
#endif  // _WIN32

  int now_date = (1900 + p.tm_year) * 10000 + (p.tm_mon + 1) * 100 + p.tm_mday;
  return now_date;
}

static inline std::string NowTimeToInt() {
  time_t now;
  time(&now);

  // choose thread safe version in each platform
  tm p;
#ifdef _WIN32
  localtime_s(&p, &now);
#else
  localtime_r(&now, &p);
#endif  // _WIN32

  int now_int = p.tm_hour * 10000 + p.tm_min * 100 + p.tm_sec;
  std::ostringstream oss;
  oss << std::setw(6) << std::setfill('0') << now_int;
  return oss.str();
}
bool directory_exists(const std::string& path) {
  struct stat info;
  if (stat(path.c_str(), &info) != 0) {
    // 无法访问路径
    return false;
  } else if (info.st_mode & S_IFDIR) {
    // 路径存在且是一个目录
    return true;
  } else {
    // 路径存在但不是一个目录
    return false;
  }
}
// 获取目录中的所有文件信息
std::vector<FileInfo> get_files_in_directory(const std::string& directory) {
  std::vector<FileInfo> files;
  DIR* dir = opendir(directory.c_str());
  if (dir != nullptr) {
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
      std::string filepath = directory + "/" + entry->d_name;
      struct stat file_stat;
      if (stat(filepath.c_str(), &file_stat) == 0 &&
          S_ISREG(file_stat.st_mode)) {
        files.push_back({filepath, file_stat.st_mtime});
      }
    }
    closedir(dir);
  }
  return files;
}

// 按文件的修改时间排序，并删除最旧的文件
void cleanup_old_logs(const std::string& logDirectory, size_t maxFiles) {
  std::vector<FileInfo> files = get_files_in_directory(logDirectory);

  // 按修改时间排序（从旧到新）
  std::sort(files.begin(), files.end(),
            [](const FileInfo& a, const FileInfo& b) {
              return a.modification_time < b.modification_time;
            });

  // 删除多余的文件
  while (files.size() > maxFiles) {
    std::cout << "Removing old log file: " << files.front().path << std::endl;
    unlink(files.front().path.c_str());
    files.erase(files.begin());
  }
}

LogWrapper::OutMode LogWrapper::GetOutModeEnum(const std::string& strMode) {
  OutMode eMode;
  if (strMode == "SYNC") {
    eMode = OutMode::SYNC;
  } else if (strMode == "ASYNC") {
    eMode = OutMode::ASYNC;
  } else {
    eMode = OutMode::SYNC;
  }

  return eMode;
}

LogWrapper::OutLevel LogWrapper::GetOutLevelEnum(const std::string& strLevel) {
  OutLevel eLevel;

  if (strLevel == "TRACE") {
    eLevel = OutLevel::LEVEL_TRACE;
  } else if (strLevel == "DEBUG") {
    eLevel = OutLevel::LEVEL_DEBUG;
  } else if (strLevel == "INFO") {
    eLevel = OutLevel::LEVEL_INFO;
  } else if (strLevel == "WARN") {
    eLevel = OutLevel::LEVEL_WARN;
  } else if (strLevel == "ERROR") {
    eLevel = OutLevel::LEVEL_ERROR;
  } else if (strLevel == "CRITI") {
    eLevel = OutLevel::LEVEL_CRITI;
  } else {
    eLevel = OutLevel::LEVEL_TRACE;
  }

  return eLevel;
}

LogWrapper::LogWrapper() : m_bInit(false) {}

LogWrapper::~LogWrapper() {
  if (m_bInit) {
    this->UnInit();
  }
}

bool LogWrapper::AddColorConsole(const char* pLoggerName,
                                 const OutLevel level) {
  printf("[log]: AddColorConsole, logName=%s  level=%d  \n", pLoggerName,
         level);
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level((spdlog::level::level_enum)level);
  // console_sink->set_pattern(LOG_OUTPUT_FORMAT);
  UpdateSinkMap(pLoggerName, console_sink);
  return true;
}

bool LogWrapper::AddRotatingFileCofig(const char* pFileName,
                                      const char* pLoggerName,
                                      const int nMaxFileSize,
                                      const int nMaxFile,
                                      const OutLevel level) {
  printf(
      "[log]: AddRotatingFileCofig, logName=%s  level=%d  fileName=%s  "
      "maxFileSize=%d  maxFile=%d \n",
      pLoggerName, level, pFileName, nMaxFileSize, nMaxFile);
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      pFileName, nMaxFileSize, nMaxFile);
  file_sink->set_level((spdlog::level::level_enum)level);
  // file_sink->set_pattern(LOG_OUTPUT_FORMAT);
  UpdateSinkMap(pLoggerName, file_sink);
  return true;
}
bool LogWrapper::AddRotatingFile(const char* pFileName, const char* logDir,
                                 const char* pLoggerName,
                                 const int nMaxFileSize, const int nMaxFile,
                                 const OutLevel level) {
  printf(
      "[log]: AddRotatingFile, logName=%s  level=%d  fileName=%s  logDir=%s "
      "maxFileSize=%d  maxFile=%d \n",
      pLoggerName, level, pFileName, logDir, nMaxFileSize, nMaxFile);
  if (!directory_exists(logDir)) {
    printf("logdir not exist!\n");
  } else {
    cleanup_old_logs(logDir, 5);
  }

  std::string logFullPath = GenerateLogFileName(pFileName, logDir);
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      logFullPath, nMaxFileSize, nMaxFile);
  file_sink->set_level((spdlog::level::level_enum)level);
  // file_sink->set_pattern(LOG_OUTPUT_FORMAT);
  UpdateSinkMap(pLoggerName, file_sink);
  return true;
}

bool LogWrapper::AddDailyFile(const char* pLoggerName, const char* pFileName,
                              const int nHour, const int nMinute,
                              const OutLevel eLevel) {
  //"%Y-%m-%d:%H:%M:%S.log"
  auto DailyFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
      pFileName, nHour, nMinute);
  DailyFileSink->set_level((spdlog::level::level_enum)eLevel);
  UpdateSinkMap(pLoggerName, DailyFileSink);
  return true;
}
std::string LogWrapper::GenerateLogFileName(const std::string& fileName,
                                            const std::string& logDir) {
  int date = NowDateToInt();
  std::string logger_full_name =
      fileName + "_" + std::to_string(date) + "_" + NowTimeToInt();
  return logDir + "/" + logger_full_name + ".log";
}

bool LogWrapper::InitConfig(const char* configFile) {
  try {
    std::ifstream inf;
    inf.open(configFile, std::ios::in);
    if (!inf.is_open()) {
      perror("Error opening file");
      return false;
    }
    json jsonRoot;
    inf >> jsonRoot;
    inf.close();

    std::string str = jsonRoot.dump();
    std::cout << "jsonRoot:" << str << std::endl;
    // Parser Data
    json jsLoggers = GetJsonValue<json>("loggers", jsonRoot);
    std::string strOutMode = GetJsonValue<std::string>("outputMode", jsonRoot);
    std::string strOutFormat =
        GetJsonValue<std::string>("outputFormat", jsonRoot);
    OutMode eOutMode = GetOutModeEnum(strOutMode);
    // Recorder
    for (int i = 0; i < jsLoggers.size(); i++) {
      json jsLogger = jsLoggers[i];
      std::string recorderName = GetJsonValue<std::string>("name", jsLogger);
      json jsSinks = GetJsonValue<json>("sinks", jsLogger);

      // Parser all sinks in recorder
      for (int j = 0; j < jsSinks.size(); j++) {
        json jsSink = jsSinks[j];
        std::string strType = GetJsonValue<std::string>("type", jsSink);
        std::string strLevel = GetJsonValue<std::string>("level", jsSink);
        LogWrapper::OutLevel eLevel = LogWrapper::GetOutLevelEnum(strLevel);
        std::string strFileName = GetJsonValue<std::string>("fileName", jsSink);
        std::string logDir = GetJsonValue<std::string>("filePath", jsSink);
        std::string fullLogPath = GenerateLogFileName(strFileName, logDir);
        if (strType == "color_console")  // cosole
        {
          AddColorConsole(recorderName.c_str(), eLevel);
        } else if (strType == "rotating_file")  // rotatiing file
        {
          int nMaxFileSize = GetJsonValue<int>("maxFileSize", jsSink);
          int nMaxFile = GetJsonValue<int>("maxFile", jsSink);
          if (!directory_exists(logDir)) {
            printf("logdir not exist!\n");
          } else {
            cleanup_old_logs(logDir, 5);
          }
          AddRotatingFileCofig(fullLogPath.c_str(), recorderName.c_str(),
                               nMaxFileSize, nMaxFile, eLevel);
        } else if (strType == "daily_file")  // date file
        {
          int nHour = GetJsonValue<int>("hour", jsSink);
          int nMinute = GetJsonValue<int>("minute", jsSink);
          AddDailyFile(recorderName.c_str(), fullLogPath.c_str(), nHour,
                       nMinute, eLevel);
        }
      }
    }
    return Init(eOutMode, strOutFormat);
  } catch (std::exception& e) {
    printf("AddConfig failed!, err:%s\n", e.what());
    return false;
  } catch (...) {
    printf("AddConfig failed!, exception\n");
    return false;
  }
  return true;
}

bool LogWrapper::Init(const OutMode outMode, const std::string strLogFormat) {
  if (m_bInit) {
    printf("It's already initialized\n");
    return false;
  }
  m_bInit = true;

  if (outMode == ASYNC)  // async
  {
    printf("[log]: mode=ASYNC \n");
    for (auto e : m_mapLoggerParam) {
      std::string strLogName = e.first;
      std::vector<spdlog::sink_ptr> vecSink(e.second);
      auto tp = std::make_shared<spdlog::details::thread_pool>(1024000, 1);
      auto logger = std::make_shared<spdlog::async_logger>(
          strLogName, begin(vecSink), end(vecSink), tp,
          spdlog::async_overflow_policy::block);
      UpdateThreadPoolMap(strLogName, tp);
      // Set log output level
      logger->set_level(spdlog::level::trace);
      // Warning log will be flush to file as soon as possible
      logger->flush_on(spdlog::level::warn);
      spdlog::register_logger(logger);
    }
  } else  // sync
  {
    printf("[log]:  mode=SYNC \n");
    for (auto e : m_mapLoggerParam) {
      std::string strLogName = e.first;
      std::vector<spdlog::sink_ptr> vecSink(e.second);
      auto logger = std::make_shared<spdlog::logger>(strLogName, begin(vecSink),
                                                     end(vecSink));
      logger->set_level(spdlog::level::trace);
      logger->flush_on(spdlog::level::warn);
      spdlog::register_logger(logger);
    }
  }

  // Every 3 seconds ,we flush log to files
  spdlog::flush_every(std::chrono::seconds(3));
  // Set log format
  spdlog::set_pattern(strLogFormat);

  return true;
}

void LogWrapper::UnInit() {
  spdlog::drop_all();
  spdlog::shutdown();
}

std::vector<std::string> LogWrapper::StringSplit(const std::string& strSrc,
                                                 const std::string& strSplit) {
  std::vector<std::string> resVec;
  if ("" == strSrc) {
    return resVec;
  }
  // 方便截取最后一段数据
  std::string strs = strSrc + strSplit;

  size_t pos = strs.find(strSplit);
  size_t size = strs.size();

  while (pos != std::string::npos) {
    std::string x = strs.substr(0, pos);
    resVec.push_back(x);
    strs = strs.substr(pos + 1, size);
    pos = strs.find(strSplit);
  }

  return resVec;
}

void LogWrapper::UpdateSinkMap(std::string recorderName,
                               spdlog::sink_ptr pSink) {
  auto iter = m_mapLoggerParam.find(recorderName);
  if (iter != m_mapLoggerParam.end()) {
    iter->second.push_back(pSink);
  } else {
    std::vector<spdlog::sink_ptr> vecSink;
    vecSink.push_back(pSink);
    m_mapLoggerParam[recorderName] = vecSink;
  }
}

void LogWrapper::UpdateThreadPoolMap(
    std::string recorderName,
    std::shared_ptr<spdlog::details::thread_pool> pThreadPool) {
  auto iter = m_mapAsyncThreadPool.find(recorderName);
  if (iter != m_mapAsyncThreadPool.end()) {
    iter->second = (pThreadPool);
  } else {
    m_mapAsyncThreadPool[recorderName] = pThreadPool;
  }
}
