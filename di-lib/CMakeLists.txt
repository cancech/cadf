cmake_minimum_required(VERSION 3.10)
include("../standalone_project.cmake")

# Configure for compilation and linking
set(component di)
configure_lib("")

# Support for test and deployment
add_subdirectory(test)
common_install()
