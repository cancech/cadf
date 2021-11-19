cmake_minimum_required(VERSION 3.10)
set(CADF_VERSION_MAJOR 0)
set(CADF_VERSION_MINOR 0)
set(CADF_VERSION_PATCH 1)
set(CADF_VERSION ${CADF_VERSION_MAJOR}.${CADF_VERSION_MINOR}.${CADF_VERSION_PATCH})
project("cadf" VERSION ${CADF_VERSION} DESCRIPTION "C++ Application Development Framework")
enable_testing()

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

# Remove 'lib' prefix for shared libraries on Windows
if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()



add_subdirectory(thread-lib)

