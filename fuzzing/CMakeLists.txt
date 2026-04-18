cmake_minimum_required(VERSION 3.10)

if(${CMAKE_VERSION} VERSION_LESS 3.10)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(FuzzSignStreamInit
        VERSION 1.0
        DESCRIPTION "Fuzzing sign-stream INIT APDU payload layout"
        LANGUAGES C)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

if (NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "Fuzzer needs to be built with Clang")
endif()

if (NOT DEFINED BOLOS_SDK)
  message(FATAL_ERROR "BOLOS_SDK environment variable not found.")
endif()

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed.")
endif()

if (NOT DEFINED ENV{LIB_FUZZING_ENGINE})
	set(COMPILATION_FLAGS_ "-g -Wall -fsanitize=fuzzer,address,undefined")
else()
	set(COMPILATION_FLAGS_ "$ENV{LIB_FUZZING_ENGINE} $ENV{CXXFLAGS}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
string(REPLACE " " ";" COMPILATION_FLAGS ${COMPILATION_FLAGS_})

include(extra/SignStreamFuzzDeps.cmake)

add_executable(fuzz_sign_stream_init fuzz_sign_stream_init.c)

target_compile_options(fuzz_sign_stream_init PUBLIC ${COMPILATION_FLAGS})
target_link_options(fuzz_sign_stream_init PUBLIC ${COMPILATION_FLAGS})
target_link_libraries(fuzz_sign_stream_init PUBLIC signstreamfuzz)
