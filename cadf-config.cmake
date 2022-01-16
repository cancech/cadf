foreach(component ${cadf_FIND_COMPONENTS})
    include(${CMAKE_CURRENT_LIST_DIR}/cadf-${component}-config.cmake)
endforeach()