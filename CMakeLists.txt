cmake_minimum_required(VERSION 3.10)
project(DupeFinder_c C)

set(CMAKE_C_STANDARD 11)
find_package(OpenSSL REQUIRED)
link_libraries(crypto)
add_executable(DupeFinder_c main.c main.h hashmap.c hashmap.h)