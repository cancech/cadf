cmake_minimum_required(VERSION 3.10)

# The extra stuff required for the thread dependencies
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Configure for compilation and linking
set(component thread)
configure_lib("Threads::Threads")

# Support for test and deployment
add_subdirectory(test)
common_install()
