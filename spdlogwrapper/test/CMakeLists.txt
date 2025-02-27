cmake_minimum_required(VERSION 3.10)


# 设置项目名称
project(test VERSION 1.0)

# 指定C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# 添加头文件路径（等价于 g++命令的 -I）
include_directories(./include)
# 添加库路径（等价于 g++命令的 -L ）
link_directories(./lib)

# 指定从一组源文件 source1 source2 … sourceN 编译出一个可执行文件
add_executable(${PROJECT_NAME} main.cpp)


if(WIN32)
  message("Running on Windows")
  # 指定目标需要链接的库（等价于 g++命令中的 -l）
target_link_libraries(${PROJECT_NAME} hlog_st)
else()
  message("Not running on Windows")
  # 指定目标需要链接的库（等价于 g++命令中的 -l）
target_link_libraries(${PROJECT_NAME} LogWrapper pthread)
endif()
