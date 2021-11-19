cmake_minimum_required(VERSION 3.10)
project(cadf-di-lib-test)

find_package (Boost 1.65 REQUIRED COMPONENTS unit_test_framework)
list(APPEND INCLUDE_DIRS ${BOOST_INCLUDE_DIRS})

file(GLOB_RECURSE sources_test src/cadf/di/*.cpp)

add_executable(cadf-di-test 
    src/testMain.cpp
    ${sources_test}
)
target_include_directories(cadf-di-test PUBLIC include)
target_link_libraries(cadf-di-test cadf-di)
target_link_libraries(cadf-di-test
  ${Boost_LIBRARIES}
  boost_unit_test_framework
)

option(ENABLE_AUTOREGISTRATION "Enable the Bean Autoregistration flag for the tests" OFF)
if(ENABLE_AUTOREGISTRATION)
	target_compile_definitions(cadf-di-test PUBLIC ENABLE_BEAN_AUTOREGISTRATION)
ENDIF(ENABLE_AUTOREGISTRATION)
