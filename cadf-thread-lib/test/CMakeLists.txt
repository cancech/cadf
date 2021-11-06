cmake_minimum_required(VERSION 3.10)
project(cadf-thread-lib-test)

set(CMAKE_THREAD_PREFER_PTHREAD ON)                                                                                                                                                                                                           
set(THREADS_PREFER_PTHREAD_FLAG ON)                                                                                                                                                                                                           
find_package(Threads REQUIRED) 
find_package (Boost 1.65 REQUIRED COMPONENTS unit_test_framework)
list(APPEND INCLUDE_DIRS ${BOOST_INCLUDE_DIRS})                                                                                                                                                                                           

file(GLOB_RECURSE sources_test src/cadf/thread/*.cpp)

add_executable(cadf-thread-test 
    src/testMain.cpp
    src/TestTask.cpp
    ${sources_test}
)
target_include_directories(cadf-thread-test PUBLIC include)
target_link_libraries(cadf-thread-test
  cadf-thread
  Threads::Threads
  ${Boost_LIBRARIES}
  boost_unit_test_framework
)
