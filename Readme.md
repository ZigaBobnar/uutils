# UUTILS

Collection of C and C++ utilities that can be used to accelerate the development of embedded projects by providing ready to use tested components.

Available utilities:
- FIFO queue [fifo.h, fifo.hpp] - cyclic buffer that provides first-in first-out capabilities)
- Checksum [checksum.h, checksum.hpp] - Calculate 8-bit sum of all bytes in the buffer
- Dynamic numeric values [dynamic_value.h, dynamic_value.hpp] - Allows the transfer of arbitary sized numeric value by using most significant bits to increase the bitness of such number (e.g. allows int64_t value 24 to be sent as single uint8_t value 24, while still allowing lots of large int64_t values (we lose some bits with serializing) to be encoded as 8 bytes). Learn more about this in the include file.
- uproto [uproto/uproto.h] - Simple protocol to allow data exchange between two or more embedded devices. It uses resource ids to determine where the message belongs (think of it as a channel that sinks messages). It also allows dynamicaly sized payload, so each resource id can also receive or send data and parse it accordingly.

## Integrating into C/C++ project

If your project uses **CMake** build system, you can add `uutils` to a directory (e.g. by git clone or directly downloading), then add uutils as subdirectory and set `UUTILS_BUILD_TESTS` option to off (this will disable the uutils tests building) by adding following to your project CMakeLists.txt file:
```
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../path/to/googletest ${CMAKE_CURRENT_BINARY_DIR}/uutils)

option(UUTILS_BUILD_TESTS "Build uutils_tests when enabled" ON)
set(UUTILS_BUILD_TESTS OFF)
```

For **PlatformIO** projects a `library.json` is present so this project can simply be cloned into lib folder and then you add `lib_deps = lib/uutils` to your platformio.ini file. Note that due to the use of C++ STL the C++ files are incompatible with AVR boards (Arduino), in this case you will be limited to the use of C interfaces.

If your project does not use CMake, you can add the uutils, then take a look into `src/CMakeLists.txt`. In the beginning of the file there are definitions for `UUTILS_C_SOURCES` and `UUTILS_CPP_SOURCES` which contain the list of C and C++ files that are required. If you only want to use C you don't have to add .cpp source files, but then you sould not use .hpp headers. Please note if you this technique you might need to recheck the sources list every time you update the uutils to newer version.

## Standalone building and testing

To build the standalone uutils project and tests CMake build system is used (with compatible C/C++ compiler). For testing [googletest](https://github.com/google/googletest) is being used. It can be obtained as a git submodule using `git submodule init` and then `git submodule update`.

Now the CMake project can be configured:
- Run `cmake -B build` to configure the project
    This will create build directory that contains configured project

Build the project:
- Run `cmake --build build`
    This builds the configuration from build directory

To test the project you can directly invoke the `uutils_tests` executable, or use ctest (e.g. run `ctest -C Debug --verbose` in the build directory).

## Project layout

The project contains the following upper level items:
```
├─ include
│  ├─ uutils
│  │  ├─ **/*.h
│  │  └─ **/*.hpp
│  └─ CMakeLists.txt
│
├─ src
│  ├─ uutils
│  │  ├─ **/*.c
│  │  └─ **/*.cpp
│  └─ CMakeLists.txt
│
├─ tests
│  ├─ uutils
│  │  └─ **/test_*.cpp
│  ├─ main.cpp
│  └─ CMakeLists.txt
│
├─ dependencies
│  └─ googletest (submodule)
│
└─ CMakeLists.txt
```
