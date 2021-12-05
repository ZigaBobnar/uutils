set(UUTILS_C_SOURCES
    uutils/uproto/message.c
    uutils/uproto/parser.c
    uutils/uproto/runtime.c
    uutils/uproto/uproto.c
    uutils/uproto/version.c

    uutils/common.c
    uutils/checksum.c
    uutils/dynamic_value.c
    uutils/fifo.c
)
if(UUTILS_BUILD_TESTS)
    set(UUTILS_C_SOURCES ${UUTILS_C_SOURCES} uutils/memory_debug.cpp)
endif()

set(UUTILS_CPP_SOURCES
    uutils/checksum.cpp
    uutils/dynamic_value.cpp
    uutils/fifo.cpp
)

set(UUTILS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

add_library(${PROJECT_NAME}
    ${UUTILS_C_SOURCES}
    ${UUTILS_CPP_SOURCES}
)
add_library(uutils::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${UUTILS_INCLUDE_DIRS}
)
