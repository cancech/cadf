# Configures the a shared library for compilation
#
# param0 - src_files - list of source files that are to be includes in the library
# param1 - link_libs - list of external libraries that this library depends on
function(configure_lib link_libs)
    file(GLOB_RECURSE src_files CONFIGURE_DEPENDS src/*.cpp)
    add_library(${PROJECT_NAME} SHARED ${src_files})
    add_library(cadf::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
    target_include_directories(${PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>)
    set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${CADF_VERSION}
                                                     SOVERSION ${CADF_VERSION_MINOR})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${link_libs})
endfunction()

# Sets up for the installation of the compiled and prepared project
function(common_install)
    # Install the binary
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
    )

    # Install the include files
    install(DIRECTORY include/ DESTINATION ${INCLUDE_INSTALL_DIR})

    #  Install the details for linking purposes
    install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}.cmake
        NAMESPACE cadf::
        DESTINATION ${LIB_INSTALL_DIR})
    configure_file(cadf::${PROJECT_NAME}-config-version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cadf::${PROJECT_NAME}-config-version.cmake @ONLY)
    install(FILES cadf::${PROJECT_NAME}-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/cadf::${PROJECT_NAME}-config-version.cmake DESTINATION ${LIB_INSTALL_DIR})
endfunction()
