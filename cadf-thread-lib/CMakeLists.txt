cmake_minimum_required(VERSION 3.10)
project(cadf-thread VERSION ${CADF_VERSION})

include(GNUInstallDirs)
set(include_dest ${CMAKE_INSTALL_INCLUDEDIR}/cadf-${CADF_VERSION})
set(lib_dest ${CMAKE_INSTALL_LIBDIR}/cadf-${CADF_VERSION})

set(CMAKE_THREAD_PREFER_PTHREAD ON)                                                                                                                                                                                                           
set(THREADS_PREFER_PTHREAD_FLAG ON)                                                                                                                                                                                                           
find_package(Threads REQUIRED) 

#Compile the Thread library                                                                                                                                                                                                       
include_directories(include)
add_library(${PROJECT_NAME} SHARED
        src/thread/BasicThreadPool.cpp
        src/thread/Task.cpp
        src/thread/Thread.cpp
)
add_library(cadf::thread ALIAS ${PROJECT_NAME})                                                                                                                                                                                          
target_include_directories(${PROJECT_NAME} PUBLIC 
                           $<BUILD_INTERFACE:${MY_LIBRARY_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:${include_dest}>)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${CADF_VERSION} 
                                      SOVERSION ${CADF_VERSION_MINOR})
add_subdirectory(test)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${lib_dest}
    ARCHIVE DESTINATION ${lib_dest}
)
INSTALL(DIRECTORY include/ DESTINATION ${include_dest})
INSTALL(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}.cmake
        DESTINATION ${lib_dest})
configure_file(cadf-thread-config-version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cadf-thread-config-version.cmake @ONLY)
INSTALL(FILES cadf-thread-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/cadf-thread-config-version.cmake DESTINATION ${lib_dest})