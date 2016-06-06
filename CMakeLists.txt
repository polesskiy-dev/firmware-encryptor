cmake_minimum_required(VERSION 3.3)
project(FirmwareEncryptorC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(SOURCE_FILES main.c hex_string.h hex_string.c system.h des.h des.c)
add_executable(FirmwareEncryptorC ${SOURCE_FILES} hex_string.h hex_string.c system.h main.c des.h des.c)

