# To be included in a project contained within cadf, to ensure that the project
# can be handled independently of the rest of the cadf, and avoid issues with
# infinite regression
if(NOT DEFINED FULL_BUILD)
    set(PROJECT_BUILD true)
    add_subdirectory(".." build)
endif()
