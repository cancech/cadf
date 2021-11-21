cmake_minimum_required(VERSION 3.10)
project(di VERSION ${CADF_VERSION})

# Configure for compilation and linking
set(SRC_FILES
		src/di/exception/InvalidBeanException.cpp
		src/di/BeanManager.cpp
		src/di/CircularDependencyChecker.cpp
		src/di/Configuration.cpp
		src/di/Context.cpp
)
configure_lib("${SRC_FILES}" "")

# Support for test and deployment
#add_subdirectory(test)
common_install()
