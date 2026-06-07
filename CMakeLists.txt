cmake_minimum_required(VERSION 3.10)
project(tcp-server)

find_package(OpenSSL REQUIRED)

add_executable(tcp-server non_blocking.cpp WS_Frame/frame.cpp)
target_link_libraries(tcp-server OpenSSL::SSL OpenSSL::Crypto)
