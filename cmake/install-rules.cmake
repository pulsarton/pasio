include_guard(GLOBAL)

if(PROJECT_IS_TOP_LEVEL)
    include(cmake/packaging.cmake)
    include(CPack)
endif()
