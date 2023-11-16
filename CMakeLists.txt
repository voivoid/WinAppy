cmake_minimum_required(VERSION 3.28)
project(WinAppy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "/W4 /WX /DNOMINMAX /DWIN32_LEAN_AND_MEAN /EHsc /MP /permissive- /volatile:iso /Zc:preprocessor,inline,lambda,__cplusplus,externConstexpr,throwingNew,templateScope,enumTypes")

option(WINAPPY_BUILD_SAMPLES "Build samples" ON)

add_subdirectory(lib)

if(WINAPPY_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

include(cmake/format.cmake)