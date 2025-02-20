include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(
    TARGETS ${PROJECT_NAME}
    RUNTIME_DEPENDENCY_SET WheelDeps
    RUNTIME DESTINATION . COMPONENT Wheel
    LIBRARY DESTINATION . COMPONENT Wheel
    ARCHIVE DESTINATION . COMPONENT Wheel
    EXCLUDE_FROM_ALL)

set(_pre_exclude
    [[api-ms-win-.*]]
    [[ext-ms-.*]]
    [[kernel32\.dll]]
    [[libc\.so\..*]]
    [[libgcc_s\.so\..*]]
    [[libm\.so\..*]]
    [[libstdc\+\+\.so\..*]])

set(_post_exclude [[.*/system32/.*\.dll]] [[^/lib.*]] [[^/usr/lib.*]])

# install(
#     RUNTIME_DEPENDENCY_SET WheelDeps
#     PRE_EXCLUDE_REGEXES [[api-ms-win-.*]]
#                         [[ext-ms-.*]]
#                         [[kernel32\.dll]]
#                         [[python.*\.dll]]
#                         [[libc\.so\..*]]
#                         [[libgcc_s\.so\..*]]
#                         [[libm\.so\..*]]
#                         [[libstdc\+\+\.so\..*]]
#                         [[python.*\.so.*]]
#     POST_EXCLUDE_REGEXES [[.*/system32/.*\.dll]]
#                          [[^/lib.*]]
#                          [[^/usr/lib.*]]
#     DIRECTORIES "${CONAN_RUNTIME_LIB_DIRS}"
#     RUNTIME DESTINATION . COMPONENT Wheel OPTIONAL EXCLUDE_FROM_ALL
#     LIBRARY DESTINATION . COMPONENT Wheel OPTIONAL EXCLUDE_FROM_ALL)

# find_package(<package>) call for consumers to find this project
set(package ${PROJECT_NAME})

install(
    TARGETS ${package}
    EXPORT ${package}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${package}_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${package}_Runtime NAMELINK_COMPONENT ${package}_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${package}_Development
    FILE_SET ${package}_Headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${package}_Development)

write_basic_package_version_file(${package}ConfigVersion.cmake COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)

# Allow package maintainers to freely override the path for the configs
set(${package}_INSTALL_CMAKEDIR
    ${CMAKE_INSTALL_DATADIR}/${package}
    CACHE STRING "CMake package config location relative to the install prefix")
set_property(CACHE ${package}_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(${package}_INSTALL_CMAKEDIR)

configure_package_config_file(cmake/install-config.cmake.in ${package}Config.cmake
                              INSTALL_DESTINATION ${${package}_INSTALL_CMAKEDIR})

install(DIRECTORY ${PROJECT_BINARY_DIR}/include/pasio DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${package}_Development)

install(FILES ${PROJECT_BINARY_DIR}/${package}Config.cmake ${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake
        DESTINATION ${${package}_INSTALL_CMAKEDIR}
        COMPONENT ${package}_Development)

install(
    FILES ${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake
    DESTINATION ${${package}_INSTALL_CMAKEDIR}
    COMPONENT ${package}_Development)

install(
    EXPORT ${package}Targets
    NAMESPACE ${package}::
    DESTINATION ${${package}_INSTALL_CMAKEDIR}
    COMPONENT ${package}_Development)

if(WIN32)
    install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${package}_Development CONFIGURATIONS Debug RelWithDebInfo)
endif()

if(PROJECT_IS_TOP_LEVEL)
    include(CPack)
endif()
