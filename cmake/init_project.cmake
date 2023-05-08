function(setup_compiler)
    if (NOT DEFINED CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
    endif()
    if (NOT CMAKE_C_COMPILER_LOADED AND NOT CMAKE_C_COMPILER)
        execute_process(COMMAND which clang OUTPUT_VARIABLE CMAKE_C_COMPILER OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
    if (NOT CMAKE_CXX_COMPILER_LOADED AND NOT CMAKE_CXX_COMPILER)
        execute_process(COMMAND which clang++ OUTPUT_VARIABLE CMAKE_CXX_COMPILER OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
endfunction()

function(print_compiler_info compiler)
    if (NOT CMAKE_${compiler}_COMPILER_LOADED)
        return()
    endif()
    message(STATUS "${compiler} compiler ${CMAKE_${compiler}_COMPILER_ID}-${CMAKE_${compiler}_COMPILER_VERSION} [${CMAKE_${compiler}_COMPILER}] selected")
    if ((compiler STREQUAL "C" AND CMAKE_COMPILER_IS_GNUCC) OR
        (compiler STREQUAL "CXX" AND CMAKE_COMPILER_IS_GNUCXX))
        message(STATUS "${compiler} compiler is part of GCC")
    endif()
endfunction()

function(init_compile_options)
    set(CXX_FLAGS "-Wall" "-Wextra" "-Werror")
    set(CXX_FLAGS_DEBUG "-g")
    set(CXX_FLAGS_RELEASE "-O3")
    
    if(NOT CMAKE_COLOR_DIAGNOSTICS)
        list(APPEND CXX_FLAGS "-fdiagnostics-color=never")
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
        list(APPEND CXX_FLAGS_DEBUG "-fno-limit-debug-info" "-fcolor-diagnostics")
    endif()

    if (NOT ${CMAKE_BUILD_TYPE} OR ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        list(APPEND CXX_FLAGS ${CXX_FLAGS_DEBUG})
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        list(APPEND CXX_FLAGS ${CXX_FLAGS_RELEASE})
    else()
        message(FATAL_ERROR "Unexpected build type: " ${CMAKE_BUILD_TYPE})
    endif()

    add_compile_options(${CXX_FLAGS})
endfunction()

function(print_compile_options)
    get_property(compile_options_list DIRECTORY PROPERTY COMPILE_OPTIONS)
    message(STATUS "compile_options: \"${compile_options_list}\"")
endfunction()