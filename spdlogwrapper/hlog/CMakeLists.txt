cmake_minimum_required(VERSION 3.10)
project(LogWrapper VERSION 1.0)

# 添加第三方库源码
add_subdirectory(./spdlog-1.x)
add_subdirectory(./json)

# 定义源码
set(SOURCES ./log_wrapper.cpp)

# 添加头文件路径
include_directories(./json/include)
include_directories(./spdlog-1.x/include)
include_directories(/usr/include/python3.8/)

# 设置windows下msvc编译时，使用动态运行时库
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

execute_process(COMMAND python3-config --cflags OUTPUT_VARIABLE PYTHON_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND python3-config --ldflags OUTPUT_VARIABLE PYTHON_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
# 分割路径
separate_arguments(PYTHON_CFLAGS)
separate_arguments(PYTHON_LDFLAGS)

include_directories(${PYTHON_CFLAGS})
link_libraries(${PYTHON_LDFLAGS})


# 添加源文件到静态库
add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_library(log_wrapper MODULE log_wrapper_bindings.cpp log_wrapper.cpp)
# 链接第三方库
target_link_libraries(${PROJECT_NAME} spdlogd nlohmann_json)

# install规则
if(WIN32)
    message("Running on Windows")
else()
    message("Not running on Windows")

    # 将include目录添加到静态库的头文件路径中
    target_include_directories(${PROJECT_NAME} PUBLIC ./json/include)
    target_include_directories(${PROJECT_NAME} PUBLIC ./spdlog-1.x/include)
    target_include_directories(${PROJECT_NAME} PUBLIC .log_wrapper.h)

    # 创建文件夹
    message(out_dir: ${PROJECT_BINARY_DIR})

    # 库安装路径
    install(FILES ./log_wrapper.h DESTINATION ${PROJECT_BINARY_DIR}/include)
    install(DIRECTORY ./json/include DESTINATION ${PROJECT_BINARY_DIR})
    install(DIRECTORY ./spdlog-1.x/include DESTINATION ${PROJECT_BINARY_DIR})
    install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION ${PROJECT_BINARY_DIR}/lib)
endif()
