# Perform the necessary steps to prepare for a test
macro(test_setup)
    enable_testing()
    find_package(Boost ${BOOST_REQ_VERSION} REQUIRED COMPONENTS unit_test_framework)
    list(APPEND INCLUDE_DIRS ${BOOST_INCLUDE_DIRS})
    
endmacro()

# Configure for building the test case
#
# param0 - link_libs - libraries that are to be linked as part of the building (except for unit test libraries)
function(configure_test link_libs)
    include_directories(../../external)
    file(GLOB_RECURSE test_src_files CONFIGURE_DEPENDS src/*.cpp)
    add_executable(${PROJECT_NAME} ${test_src_files})
    target_include_directories(${PROJECT_NAME} PUBLIC include)
    target_link_libraries(${PROJECT_NAME}
        ${link_libs}
        boost_unit_test_framework
    )
    add_test(${PROJECT_NAME} ${PROJECT_NAME})
endfunction()
