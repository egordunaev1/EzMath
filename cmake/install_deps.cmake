function(install_gtest)
    message("[GTest] Fetching...")
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    message("[GTest] Done")
endfunction()

function(install_fmt)
    message("[fmt] Fetching...")
    include(FetchContent)
    FetchContent_Declare(fmt
      GIT_REPOSITORY https://github.com/fmtlib/fmt.git
      GIT_TAG master
    )
    FetchContent_MakeAvailable(fmt)
    message("[fmt] Done")
endfunction()