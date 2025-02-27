#include <pybind11/pybind11.h>

#include "log_wrapper.h"

namespace py = pybind11;

PYBIND11_MODULE(log_wrapper, m) {
  py::enum_<LogWrapper::OutMode>(m, "OutMode")
      .value("SYNC", LogWrapper::OutMode::SYNC)
      .value("ASYNC", LogWrapper::OutMode::ASYNC)
      .export_values();
  py::enum_<LogWrapper::OutLevel>(m, "OutLevel")
      .value("LEVEL_TRACE", LogWrapper::OutLevel::LEVEL_TRACE)
      .value("LEVEL_DEBUG", LogWrapper::OutLevel::LEVEL_DEBUG)
      .value("LEVEL_INFO", LogWrapper::OutLevel::LEVEL_INFO)
      .value("LEVEL_WARN", LogWrapper::OutLevel::LEVEL_WARN)
      .value("LEVEL_ERROR", LogWrapper::OutLevel::LEVEL_ERROR)
      .value("LEVEL_CRITI", LogWrapper::OutLevel::LEVEL_CRITI)
      .value("LEVEL_OFF", LogWrapper::OutLevel::LEVEL_OFF)
      .export_values();

  py::class_<LogWrapper>(m, "LogWrapper")
      .def(py::init<>())
      .def("AddColorConsole", &LogWrapper::AddColorConsole,
           py::arg("recorderName") = "log",
           py::arg("level") = LogWrapper::LEVEL_TRACE)
      .def("AddRotatingFileCofig", &LogWrapper::AddRotatingFileCofig,
           py::arg("pFileName"), py::arg("recorderName") = "log",
           py::arg("nMaxFileSize") = 1024 * 1024 * 10, py::arg("nMaxFile") = 20,
           py::arg("level") = LogWrapper::LEVEL_TRACE)
      .def("AddRotatingFile", &LogWrapper::AddRotatingFile,
           py::arg("pFileName"), py::arg("logDir"),
           py::arg("recorderName") = "log",
           py::arg("nMaxFileSize") = 1024 * 1024 * 10, py::arg("nMaxFile") = 20,
           py::arg("level") = LogWrapper::LEVEL_TRACE)
      .def("AddDailyFile", &LogWrapper::AddDailyFile, py::arg("recorderName"),
           py::arg("pFileName"), py::arg("nHour"), py::arg("nMinute"),
           py::arg("eLevel") = LogWrapper::LEVEL_TRACE)
      .def("InitConfig", &LogWrapper::InitConfig, py::arg("configFile"))
      .def("Init", &LogWrapper::Init, py::arg("outMode") = LogWrapper::ASYNC,
           py::arg("strLogFormat") = LOG_OUTPUT_FORMAT)
      .def("log_trace",
           [](LogWrapper &self, const std::string &format,
              const py::args &args) {
             std::string message = py::str(format).format(*args);
             self.LogTrace(message);
           })
      .def("log_debug",
           [](LogWrapper &self, const std::string &format,
              const py::args &args) {
             std::string message = py::str(format).format(*args);
             self.LogDebug(message);
           })
      .def("log_info",
           [](LogWrapper &self, const std::string &format,
              const py::args &args) {
             std::string message = py::str(format).format(*args);
             self.LogInfo(message);
           })
      .def("log_warn",
           [](LogWrapper &self, const std::string &format,
              const py::args &args) {
             std::string message = py::str(format).format(*args);
             self.LogWarn(message);
           })
      .def("log_error",
           [](LogWrapper &self, const std::string &format,
              const py::args &args) {
             std::string message = py::str(format).format(*args);
             self.LogError(message);
           })
      .def("UnInit", &LogWrapper::UnInit);
  ;
}
