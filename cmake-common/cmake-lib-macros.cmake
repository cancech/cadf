# Configures the a shared library for compilation
#
# param0 - link_libs - list of external libraries that this library depends on
function(configure_lib link_libs)
    file(GLOB_RECURSE src_files CONFIGURE_DEPENDS src/*.cpp)
    add_library(${component} SHARED ${src_files})
    add_library(${project}::${component} ALIAS ${component})
    target_include_directories(${component} PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>)
    set_target_properties(${component} PROPERTIES VERSION ${CADF_VERSION}
                                                     SOVERSION ${CADF_VERSION_MINOR})
    target_link_libraries(${component} PUBLIC ${link_libs})
endfunction()

# Sets up for the installation of the compiled and prepared project
function(common_install)
    # Install the binary
    install(TARGETS ${component} EXPORT ${component}-targets
        COMPONENT ${component}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    # Install the include files
    install(DIRECTORY include/ DESTINATION ${INCLUDE_INSTALL_DIR})

    # Install the details for linking purposes
    install(EXPORT ${component}-targets
        FILE ${project}-${component}-config-targets.cmake
        NAMESPACE ${project}::
        DESTINATION ${LIB_INSTALL_DIR}
        COMPONENT ${component}
    )
    
    # Prepare the configuration of the component
    configure_file("${component}-config.cmake.in"
      "${CMAKE_BINARY_DIR}/${project}-${component}-config.cmake"
      @ONLY
    )

    # Prepare the version information of the component
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
      "${CMAKE_BINARY_DIR}/${project}-${component}-config-version.cmake"
      VERSION ${version}
      COMPATIBILITY AnyNewerVersion
    )
    
    # Install the component support files
    install(
      FILES
        "${CMAKE_BINARY_DIR}/${project}-${component}-config.cmake"
        "${CMAKE_BINARY_DIR}/${project}-${component}-config-version.cmake"
      DESTINATION ${LIB_INSTALL_DIR}
      COMPONENT ${component}
    )
endfunction()
