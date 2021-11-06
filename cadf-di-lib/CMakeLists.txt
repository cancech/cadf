cmake_minimum_required(VERSION 3.10)
project(cadf-di-lib)

#Compile the CORM library
include_directories(include)
add_library(cadf-di SHARED
		src/camb/di/exception/InvalidBeanException.cpp
		src/camb/di/BeanManager.cpp
		src/camb/di/CircularDependencyChecker.cpp
		src/camb/di/Configuration.cpp
		src/camb/di/Context.cpp
)
target_compile_options(cadf-di PUBLIC -Wall -c -fmessage-length=0 -fPIC)

add_subdirectory(test)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
