cmake_minimum_required(VERSION 3.10)

if(${CMAKE_VERSION} VERSION_LESS 3.10)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(unit_tests
        VERSION 0.1
        DESCRIPTION "Unit tests for WAVES Ledger application"
        LANGUAGES C)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

include(CTest)
ENABLE_TESTING()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
add_definitions(-DPRINTF=)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -pedantic -g -O0 --coverage")

set(GCC_COVERAGE_LINK_FLAGS "--coverage -lgcov")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed.")
endif()

add_compile_definitions(TEST)

include_directories(../src)
include_directories($ENV{BOLOS_SDK}/lib_standard_app)

add_executable(test_buffer_init_layout test_buffer_init_layout.c)
add_executable(test_path_policy test_path_policy.c
               ../src/crypto/path_policy.c)

# buffer.c depends on varint_read() from varint.c (ledger-secure-sdk)
add_library(buffer SHARED
            $ENV{BOLOS_SDK}/lib_standard_app/buffer.c
            $ENV{BOLOS_SDK}/lib_standard_app/varint.c)
add_library(bip32 SHARED $ENV{BOLOS_SDK}/lib_standard_app/bip32.c)
add_library(read SHARED $ENV{BOLOS_SDK}/lib_standard_app/read.c)
add_library(write SHARED $ENV{BOLOS_SDK}/lib_standard_app/write.c)

target_link_libraries(test_buffer_init_layout PUBLIC
                      buffer
                      bip32
                      cmocka
                      gcov
                      write
                      read)

target_link_libraries(test_path_policy PUBLIC cmocka gcov)

add_test(test_buffer_init_layout test_buffer_init_layout)
add_test(test_path_policy test_path_policy)
