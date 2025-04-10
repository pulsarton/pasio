cmake_minimum_required(VERSION 3.14)

set(PARENT_PROJECT_NAME ${PROJECT_NAME})

project(${PARENT_PROJECT_NAME}-tests LANGUAGES CXX)

include(../../cmake/project-is-top-level.cmake)
include(../../cmake/folders.cmake)
include(../../cmake/static-analyzers.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
    find_package(pasio REQUIRED)
    enable_testing()
endif()

# ---- Find testing frameworks

find_package(Catch2 REQUIRED)
include(Catch)

# ---- Collect test files

file(GLOB TEST_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# ---- Tests ----

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain Python3::Python)

target_link_directories(${PROJECT_NAME} PRIVATE ${Python3_RUNTIME_LIBRARY_DIRS})

target_sources(${PROJECT_NAME} PRIVATE ${TEST_SOURCES})

# ---- Collect runtime

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/)
endif()

# ---- Discover Catch2 tests

catch_discover_tests(${PROJECT_NAME})

# ---- Add coverage

if(${PARENT_PROJECT_NAME}_ENABLE_COVERAGE)
    add_coverage_lcov_target(${PROJECT_NAME})
    add_coverage_llvm_target(${PROJECT_NAME})
endif()

# ---- End-of-file commands ----

add_folders(Test)
