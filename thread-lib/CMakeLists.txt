cmake_minimum_required(VERSION 3.10)
project(thread VERSION ${CADF_VERSION})

# The extra stuff required for the thread dependencies
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Configure for compilation and linking
set(SRC_FILES 
        src/thread/BasicThreadPool.cpp
        src/thread/Task.cpp
        src/thread/Thread.cpp
)
set(LINK_LIBS Threads::Threads)
configure_lib("${SRC_FILES}" "${LINK_LIBS}")

# Support for test and deployment
add_subdirectory(test)
common_install()
