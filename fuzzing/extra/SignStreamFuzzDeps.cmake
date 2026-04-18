# Fuzz harness dependencies (sign-stream INIT payload parsing + path policy).
project(SignStreamFuzz
        VERSION 1.0
        DESCRIPTION "Waves sign-stream INIT fuzz target"
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
add_definitions(-DPRINTF=)
set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -Wno-unused-function -DFUZZ -pedantic -g -O0"
)

add_library(signstreamfuzz
    ${BOLOS_SDK}/lib_standard_app/buffer.c
    ${BOLOS_SDK}/lib_standard_app/read.c
    ${BOLOS_SDK}/lib_standard_app/bip32.c
    ${BOLOS_SDK}/lib_standard_app/write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/crypto/path_policy.c
)

set_target_properties(signstreamfuzz PROPERTIES SOVERSION 1)

target_include_directories(signstreamfuzz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${BOLOS_SDK}/lib_standard_app
)
