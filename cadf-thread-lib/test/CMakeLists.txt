cmake_minimum_required(VERSION 3.10)
project(cadf-thread-test)
enable_testing()

find_package (Boost 1.65 REQUIRED COMPONENTS unit_test_framework)
list(APPEND INCLUDE_DIRS ${BOOST_INCLUDE_DIRS})                                                                                                                                                                                           

file(GLOB_RECURSE sources_test src/thread/*.cpp)

add_executable(${PROJECT_NAME} 
    src/testMain.cpp
    src/TestTask.cpp
    ${sources_test}
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME}
  cadf::thread
  boost_unit_test_framework
)
add_test(thread-test ${PROJECT_NAME})
