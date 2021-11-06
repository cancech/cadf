cmake_minimum_required(VERSION 3.10)
project(cadf-di-lib)

#Compile the CORM library
include_directories(include)
add_library(cadf-di SHARED
		src/di/exception/InvalidBeanException.cpp
		src/di/BeanManager.cpp
		src/di/CircularDependencyChecker.cpp
		src/di/Configuration.cpp
		src/di/Context.cpp
)
target_compile_options(cadf-di PUBLIC -Wall -c -fmessage-length=0 -fPIC)

add_subdirectory(test)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
