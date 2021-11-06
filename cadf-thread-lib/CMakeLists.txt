cmake_minimum_required(VERSION 3.10)
project(cadf-thread-lib)

#Compile the Thread library                                                                                                                                                                                                       
include_directories(include)
add_library(cadf-thread SHARED
        src/cadf/thread/BasicThreadPool.cpp
        src/cadf/thread/Task.cpp
        src/cadf/thread/Thread.cpp
)
target_compile_options(cadf-thread PUBLIC -Wall -c -fmessage-length=0 -fPIC)

add_subdirectory(test)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
