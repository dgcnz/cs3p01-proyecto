cmake_minimum_required(VERSION 3.2)
project(cs3p01-proyecto)

# Require C++17.
# Change this value to require other C++ standards (14, 17, etc.)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)

include_directories(include)

# Use globbing to find source files.
# Any source files added to include/ or src/ will automatically be added to the project.
file(GLOB_RECURSE INCLUDE_FILES ${CMAKE_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SOURCE_FILES_NO_MAIN ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM SOURCE_FILES_NO_MAIN ${CMAKE_SOURCE_DIR}/src/main.cpp)
set(SOURCE_FILES ${SOURCE_FILES_NO_MAIN} ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_executable(cs3p01-proyecto ${INCLUDE_FILES} ${SOURCE_FILES_NO_MAIN} ${SOURCE_FILES})

# If external libraries are needed, use find_library() and target_link_libraries()
# to dynamically link them to your executable. Generally, you'll want to install
# them onto the host using your OS's package manager.
# Example:
#   find_library(LIB_FMT fmt)
#   target_link_libraries(cs3p01-proyecto ${LIB_FMT})

# Include extra CMake files that are used for testing and linting.
include(${CMAKE_SOURCE_DIR}/cmake/unit_test.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/clang_tools.cmake)

# Enable all warnings except for no-sign-compare, which Clang disables
# by default.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare")

option(WERROR "Turn warnings into errors" OFF)
if (WERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif ()

# OMP
find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
