cmake_minimum_required(VERSION 3.10)
project(tcp-server)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)

# add_executable(tcp-server non_blocking.cpp WS_Frame/frame.cpp WS_Frame/frame-send.cpp http.cpp ws.cpp libs/sqlite/sqlite3.c )
file(GLOB_RECURSE SOURCES "*.cpp" "*.c")
list(FILTER SOURCES EXCLUDE REGEX "build/.*")
add_executable(tcp-server ${SOURCES})
target_include_directories(tcp-server PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(tcp-server OpenSSL::SSL OpenSSL::Crypto )
