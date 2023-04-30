cmake_minimum_required(VERSION 3.20)
project(EzMath)

#=============================================================#
#                       Declare Options                       #
#=============================================================#

option(ENABLE_TESTS "Enables tests" ON)


#=============================================================#
#                       Include Modules                       #
#=============================================================#

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(init_project)
include(install_deps)


#=============================================================#
#                       Setup compilers                       #
#=============================================================#

set(CMAKE_CXX_STANDARD 23)

setup_compiler()
print_compiler_info(C)
print_compiler_info(CXX)

init_compile_options()
print_compile_options()


#=============================================================#
#                      Configure Project                      #
#=============================================================#

install_fmt()
add_subdirectory(src)


#=============================================================#
#                           Tests                             #
#=============================================================#

if(ENABLE_TESTS)
    install_gtest()
    add_subdirectory(test)
endif()