cmake_minimum_required(VERSION 3.5)

project(LOG)
set(CMAKE_CXX_FLAGS " -pthread")

set(LOG_SOUCE_PATH "./")

include_directories(./)

add_library(log SHARED spdlog.cpp)
add_executable(app_log main.cpp)
target_link_libraries(app_log log)
