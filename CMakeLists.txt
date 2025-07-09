cmake_minimum_required(VERSION 3.10)  # 指定所需的最低CMake版本
project(StudentSystem)                 # 定义项目名称

set(CMAKE_CXX_STANDARD 11)            # 设置C++标准为C++11

# 设置头文件搜索路径为当前源码目录，方便包含本地头文件
include_directories(${CMAKE_SOURCE_DIR})

# 生成 server 可执行文件，包含 server.cpp 和 student.cpp
add_executable(server server.cpp student.cpp)

# 生成 client 可执行文件，只需 client.cpp
add_executable(client client.cpp)

add_executable(addInfo addInfo.cpp)