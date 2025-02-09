option(${PROJECT_NAME}_ENABLE_IPO "enable interprocedural optimization can be enabled")

macro(enable_ipo)
    if(${PROJECT_NAME}_ENABLE_IPO)
        include(CheckIPOSupported)
        check_ipo_supported(RESULT result OUTPUT output)
        if(result)
            set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
        else()
            message(WARNING "IPO is not supported: ${output}")
        endif()
    endif()
endmacro()
