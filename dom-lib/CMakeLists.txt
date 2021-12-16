cmake_minimum_required(VERSION 3.10)
include("../standalone_project.cmake")
project(dom VERSION ${CADF_VERSION})

# Configure for compilation and linking
configure_lib("")

# Support for test and deployment
add_subdirectory(test)
common_install()
