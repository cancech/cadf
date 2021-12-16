cmake_minimum_required(VERSION 3.10)

###############################################################################
# Helper marco to define variable in both a local and parent scope as necessary
#
# - param0 - var - the name of the variable
# - param1 - value - the value to assign to the variable
###############################################################################
macro(def_var_with_parent var value)
    set(${var} ${value})
    if (DEFINED PROJECT_BUILD)
        set(${var} ${value} PARENT_SCOPE)
    endif()
endmacro()

###############################################################################
# Details of the version and overall framework
###############################################################################
def_var_with_parent(CADF_VERSION_MAJOR 0)
def_var_with_parent(CADF_VERSION_MINOR 0)
def_var_with_parent(CADF_VERSION_PATCH 1)
def_var_with_parent(CADF_VERSION ${CADF_VERSION_MAJOR}.${CADF_VERSION_MINOR}.${CADF_VERSION_PATCH})

###############################################################################
# Details of the framework project
###############################################################################
project("cadf" VERSION ${CADF_VERSION} DESCRIPTION "C++ Application Development Framework")
enable_testing()

###############################################################################
# General settings for building
###############################################################################
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_COLOR_MAKEFILE   ON)

###############################################################################
# Remove 'lib' prefix for shared libraries on Windows
###############################################################################
if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

###############################################################################
# General settings for deploying
###############################################################################
include(GNUInstallDirs)
def_var_with_parent(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/cadf-${CADF_VERSION})
def_var_with_parent(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cadf-${CADF_VERSION})

###############################################################################
# External dependency versions
###############################################################################
set (BOOST_REQ_VERSION "1.65")

###############################################################################
# Ensure that the common scripts and support files are made available
###############################################################################
add_subdirectory(cmake-common)

###############################################################################
# Add the contained sub-projects so that they can all be included in a full
# cadf build
###############################################################################
if (NOT DEFINED PROJECT_BUILD)
    set(FULL_BUILD true)
    # Add all of the libraries that create the framework
    add_subdirectory(thread-lib)
    add_subdirectory(di-lib)
    add_subdirectory(dom-lib)
endif()
