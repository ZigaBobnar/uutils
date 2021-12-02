#
# Unit testing
#
enable_testing()

set(INSTALL_GTEST OFF)
set(gtest_force_shared_crt ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest)

#
# Add unit tests executable
#
add_executable(uutils_tests
    main.cpp

    uutils/uproto/test_uproto_message.cpp
    uutils/uproto/test_uproto_parser.cpp

    uutils/test_checksum.cpp
    uutils/test_dynamic_value.cpp
    uutils/test_fifo.cpp
)

target_link_libraries(uutils_tests
    PUBLIC
        uutils
    PRIVATE
        gtest
)

add_test(
    NAME uutils_tests
    COMMAND uutils_tests
)
