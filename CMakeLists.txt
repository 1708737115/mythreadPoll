# 设置打印编译信息
set(CMAKE_VERBOSE_MAKEFILE ON)

# 设置编译器最低版本
cmake_minimum_required(VERSION 3.22.1)

# 设置C和C++编译器
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

# 添加编译选项
add_definitions("-Wall -g")

# 设置项目名称
project(thread_pool)

# 设置默认编译的C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 默认库文件的输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# 默认可执行文件的输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# 编译ini库
file(GLOB INI_SOURCES "${PROJECT_SOURCE_DIR}/src/utils/*.cpp")
add_library(ini SHARED ${INI_SOURCES})

# 编译threadpool库
file(GLOB THREAD_SOURCES "${PROJECT_SOURCE_DIR}/src/thread/*.cpp")
add_library(threadpool SHARED ${THREAD_SOURCES})

# 添加可执行文件
add_executable(test ${PROJECT_SOURCE_DIR}/test/initest.cpp)
add_executable(thread_test ${PROJECT_SOURCE_DIR}/test/threadtest.cpp)

# 链接库
target_link_libraries(thread_test PRIVATE ini threadpool)
target_link_libraries(test PRIVATE ini threadpool)