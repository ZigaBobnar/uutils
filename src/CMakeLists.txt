add_library(${PROJECT_NAME}
    uutils/common.c
    uutils/checksum.c
    uutils/checksum.cpp
    uutils/dynamic_value.c
    uutils/dynamic_value.cpp
)
add_library(uutils::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_BINARY_DIR}/include
)
