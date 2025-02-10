cmake_minimum_required(VERSION 3.14)

set(PARENT_PROJECT_NAME ${PROJECT_NAME})

project(${PARENT_PROJECT_NAME}-pytests)

include(../../cmake/project-is-top-level.cmake)
include(../../cmake/folders.cmake)
include(../../cmake/static-analyzers.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
    find_package(pasio REQUIRED)
    enable_testing()
endif()

# ---- Collect test files

file(GLOB TEST_SOURCES ${PROJECT_SOURCE_DIR}/src/*.py)

# ---- Tests ----

foreach(file ${TEST_SOURCES})
    get_filename_component(_filename ${file} NAME_WE)
    add_test(NAME ${PROJECT_NAME}-${_filename} 
             COMMAND ${Python3_EXECUTABLE} ${file}
             DEPENDS ${PARENT_PROJECT_NAME}
             WORKING_DIRECTORY $<TARGET_FILE_DIR:${PARENT_PROJECT_NAME}>)
    set_tests_properties(${PROJECT_NAME}-${_filename} PROPERTIES ENVIRONMENT "PYTHONPATH=$<TARGET_FILE_DIR:${PARENT_PROJECT_NAME}>")
endforeach()

# ---- End-of-file commands ----

add_folders(Test)
