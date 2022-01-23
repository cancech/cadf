cmake_minimum_required(VERSION 3.10)

# Configure for compilation and linking
set(component comms)
configure_lib("cadf::thread")

# Support for test and deployment
add_subdirectory(test)
common_install()
