cmake_minimum_required(VERSION 3.13)
project("uutils"
    LANGUAGES C CXX
    VERSION 0.1.0)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # This allows new policy to override option variables using normal variables
cmake_policy(SET CMP0077 NEW) # This allows new policy to override option variables using normal variables

option(UUTILS_BUILD_TESTS "Build uutils_tests when enabled" ON)

if(UUTILS_BUILD_TESTS)
    add_definitions(-DDEBUG)
endif()

add_subdirectory(src)

if(UUTILS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
