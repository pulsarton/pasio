cmake_minimum_required(VERSION 3.15)

include(GenerateExportHeader)

include(../cmake/folders.cmake)
include(../cmake/linker.cmake)
include(../cmake/sanitizers.cmake)
include(../cmake/static-analyzers.cmake)

set(PARENT_PROJECT_NAME ${PROJECT_NAME})

project(
    core
    VERSION ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}
    LANGUAGES CXX)

include(cmake/options.cmake)

# ---- Project options target

add_library(${PROJECT_NAME}-options INTERFACE)

# ---- Link with project wide options

target_link_libraries(${PROJECT_NAME}-options INTERFACE $<BUILD_INTERFACE:${PARENT_PROJECT_NAME}-options>)

# ---- Add sanitizers

target_sanitizers(
    ${PROJECT_NAME}-options
    SANITIZER_ADDRESS
    ${${PROJECT_NAME}_SANITIZER_ADDRESS}
    SANITIZER_LEAK
    ${${PROJECT_NAME}_SANITIZER_LEAK}
    SANITIZER_UNDEFINED_BEHAVIOR
    ${${PROJECT_NAME}_SANITIZER_UNDEFINED_BEHAVIOR}
    SANITIZER_THREAD
    ${${PROJECT_NAME}_SANITIZER_THREAD}
    SANITIZER_MEMORY
    ${${PROJECT_NAME}_SANITIZER_MEMORY})

# ---- Declare library precompiled headers

add_library(${PROJECT_NAME}-pch INTERFACE) # declare library if pch is off prevent install rule error for unknown target
if(${PROJECT_NAME}_ENABLE_PCH)
    target_precompile_headers(${PROJECT_NAME}-pch INTERFACE <string> <vector>)
    target_link_libraries(${PROJECT_NAME}-pch INTERFACE $<BUILD_INTERFACE:${PROJECT_NAME}-options>)
endif()

# ---- Declare object library to use in tests and module

add_library(${PROJECT_NAME}-objects OBJECT)

# ---- Collect sources

file(GLOB LIBRARY_INCLUDES ${PROJECT_SOURCE_DIR}/include/pasio/${PROJECT_NAME}/*.hpp)
file(GLOB LIBRARY_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# ---- Set target sources

target_sources(
    ${PROJECT_NAME}-objects
    PUBLIC FILE_SET
    ${PROJECT_NAME}_Headers TYPE HEADERS BASE_DIRS ${PROJECT_SOURCE_DIR}/include FILES ${LIBRARY_INCLUDES}
    PRIVATE ${LIBRARY_SOURCES})

# ---- Set target include dirs

target_include_directories(
    ${PROJECT_NAME}-objects ${warning_guard}
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# ---- Add libraries

target_link_libraries(
    ${PROJECT_NAME}-objects
    PUBLIC asio::asio pybind11::module Python3::Module
    PRIVATE $<BUILD_INTERFACE:${PROJECT_NAME}-pch> $<BUILD_INTERFACE:${PROJECT_NAME}-options>)

# ---- Declare library ----

pybind11_add_module(${PROJECT_NAME} $<TARGET_OBJECTS:${PROJECT_NAME}-objects>)
add_library(${PARENT_PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC asio::asio pybind11::module Python3::Module $<BUILD_INTERFACE:${PROJECT_NAME}-objects>
                    PRIVATE $<BUILD_INTERFACE:${PROJECT_NAME}-pch> $<BUILD_INTERFACE:${PROJECT_NAME}-options>)

# ---- Configure linker

target_configure_linker(${PROJECT_NAME})

# ---- Collect all sources for static analysis

set(ALL_SOURCE_FILES ${LIBRARY_INCLUDES} ${LIBRARY_SOURCES})

# ---- Clang-tidy target

if(${PROJECT_NAME}_ENABLE_CLANGTIDY)
    target_clangtidy(${PROJECT_NAME} WARNINGS_AS_ERRORS "${WARNINGS_AS_ERRORS}")
endif()

# ---- Cppcheck target

if(${PROJECT_NAME}_ENABLE_CPPCHECK)
    target_cppcheck(${PROJECT_NAME} WARNINGS_AS_ERRORS "${WARNINGS_AS_ERRORS}")
endif()

# ---- IWYU target

if(${PROJECT_NAME}_ENABLE_IWYU)
    target_include_what_you_use(${PROJECT_NAME} USE_ON_BUILD)
endif()

# ---- Add coverage support

if(${PROJECT_NAME}_ENABLE_COVERAGE)
    target_add_coverage_flags(${PROJECT_NAME})
endif()

# ---- Doxygen

if(${PROJECT_NAME}_BUILD_DOCS)
    include(../cmake/docs.cmake)
    wrap_doxygen_add_docs(${PROJECT_NAME} SOURCE_DIRS "${ALL_SOURCE_FILES}" OUTPUT "doxygen")
endif()

# ---- Build examples

if(${PROJECT_NAME}_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

# ---- Build tests

include(CTest)
if(BUILD_TESTING AND ${PROJECT_NAME}_BUILD_TESTING)
    add_subdirectory(test)
endif()

include(CTest)
if(BUILD_TESTING AND ${PROJECT_NAME}_BUILD_PYTESTING)
    add_subdirectory(pytest)
endif()

# ---- Configure install rules

if(NOT CMAKE_SKIP_INSTALL_RULES AND NOT ${PROJECT_NAME}_SKIP_INSTALL_RULES)
    include(cmake/install-rules.cmake)
endif()

# ---- Project outline

add_folders(${PROJECT_NAME})
