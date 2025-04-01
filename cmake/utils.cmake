function(pasio_get_version VERSION)
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml" CONTENTS)

    string(REGEX MATCH "version = \"([^\"]+)\"" MATCHED_VERSION "${CONTENTS}")

    if(MATCHED_VERSION)
        set(${VERSION}
            "${CMAKE_MATCH_1}"
            PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Failed to find version declaration in ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml")
    endif()
endfunction()
