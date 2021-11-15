cmake_minimum_required(VERSION 3.10)
project(cadf-thread VERSION ${CADF_VERSION})

#Compile the Thread library                                                                                                                                                                                                       
include_directories(include)
add_library(${PROJECT_NAME} SHARED
        src/thread/BasicThreadPool.cpp
        src/thread/Task.cpp
        src/thread/Thread.cpp
)
add_library(cadf::thread ALIAS ${PROJECT_NAME})                                                                                                                                                                                          
target_include_directories(${PROJECT_NAME} PUBLIC include)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${CADF_VERSION} 
                                      SOVERSION ${CADF_VERSION_MINOR})
add_subdirectory(test)

include(GNUInstallDirs)
install(
    TARGETS ${PROJECT_NAME}
    EXPORT cadf-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
INSTALL(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cadf-${CADF_VERSION})
