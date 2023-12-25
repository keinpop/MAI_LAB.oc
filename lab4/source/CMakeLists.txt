cmake_minimum_required(VERSION 3.17)
project(5_lab LANGUAGES C VERSION 0.1.0 DESCRIPTION "laboratory work on course OS")

set(CMAKE_C_STANDARD 99)


add_executable(statMain src/statMain.c)
add_executable(dynMain src/dynMain.c)


add_library(first SHARED
        src/realization1.c
        )


set_target_properties(first PROPERTIES
        VERSION ${PROJECT_VERSION}
        PUBLIC_HEADER src/realization.h
)


add_library(
        second SHARED
        src/realization2.c
)

set_target_properties(second PROPERTIES
        VERSION ${PROJECT_VERSION}
        PUBLIC_HEADER src/realization.h
        )


target_link_libraries(first m)
target_link_libraries(second m)


target_include_directories(first PRIVATE src/)
target_include_directories(second PRIVATE src/)

set(custom_targets)

list(APPEND custom_targets first)
list(APPEND custom_targets second)


if(NOT lab_link_type)
    set(lab_link_type first)
endif()

function(invalid_target)
    message("Unknown target selected")
    message("All available targets:")
    foreach(target IN ITEMS ${custom_targets})
        message("    ${target}")
    endforeach()
endfunction()


if (${lab_link_type} IN_LIST custom_targets)
else()
    invalid_target()
    message( FATAL_ERROR "Invalid target selected" )
endif()

message("Link library for statMain will be now " ${lab_link_type})

target_link_libraries(statMain ${lab_link_type})
target_link_libraries(dynMain ${CMAKE_DL_LIBS})
add_dependencies(dynMain first second)