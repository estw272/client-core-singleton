set(CMAKE_GENERATOR "Ninja")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#library symbol visibility
# Hide things by default
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

#minimum c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#output dirs
include(GNUInstallDirs)
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
            ${CMAKE_HOME_DIRECTORY}/build$<$<CONFIG:Debug>:/debug>$<$<CONFIG:Release>:/release>/${PROJECT_NAME})
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
            ${CMAKE_HOME_DIRECTORY}/build$<$<CONFIG:Debug>:/debug>$<$<CONFIG:Release>:/release>/${PROJECT_NAME})
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
            ${CMAKE_HOME_DIRECTORY}/build$<$<CONFIG:Debug>:/debug>$<$<CONFIG:Release>:/release>/${PROJECT_NAME})
endif()
set(CMAKE_DEBUG_POSTFIX d)

#enable unicode
add_definitions(-DUNICODE -D_UNICODE)

# set(BUILD_SHARED_LIBS OFF)

# Generate compile_commends.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_IPO
       "Enable Iterprocedural Optimization, aka Link Time Optimization (LTO)"
       ON)

if(ENABLE_IPO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT output)
    if(result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(SEND_ERROR "IPO is not supported: ${output}")
    endif()
endif()