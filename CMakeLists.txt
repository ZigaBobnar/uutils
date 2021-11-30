cmake_minimum_required(VERSION 3.13)
project("uutils"
    LANGUAGES C CXX
    VERSION 1.2.0)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # This allows new policy to override option variables using normal variables
cmake_policy(SET CMP0077 NEW) # This allows new policy to override option variables using normal variables

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
