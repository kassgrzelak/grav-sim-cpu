cmake_minimum_required(VERSION 3.20)
project(grav_sim_cpu)
set(CMAKE_CXX_STANDARD 20)

option(DEV_BUILD OFF)

if (CMAKE_BUILD_TYPE MATCHES Release)
    if (MSVC)
        add_compile_options(/O2)
    else()
        if (DEV_BUILD)
            add_compile_options(-O3 -march=native -mtune=native)
        else()
            add_compile_options(-O3)
        endif()
    endif()
endif()

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(raylib QUIET)
if (NOT raylib_FOUND)
    message(STATUS "Fetching Raylib...")
    FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.5
    )
    FetchContent_MakeAvailable(raylib)
endif()

find_package(glm QUIET)
if(NOT glm_FOUND)
    message(STATUS "Fetching GLM...")
    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG 1.0.3
    )
    FetchContent_MakeAvailable(glm)
endif()

find_package(TBB QUIET)
if(NOT TBB_FOUND)
    message(STATUS "Fetching TBB...")
    FetchContent_Declare(
            oneTBB
            GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
            GIT_TAG v2021.11.0
    )
    FetchContent_MakeAvailable(oneTBB)
endif()

# Normalize GLM target
if(TARGET glm::glm)
    set(GLM_TARGET glm::glm)
elseif(TARGET glm)
    set(GLM_TARGET glm)
else()
    message(FATAL_ERROR "GLM not found or fetched.")
endif()

add_executable(grav_sim_cpu main.cpp
        include/parameters.hpp
        include/Sim.hpp
        include/BodyGenerator.hpp
        src/BodyGenerator.cpp
        src/Sim.cpp
        include/QuadTree.hpp
        src/QuadTree.cpp
        include/common.hpp
        src/parameters.cpp
        include/colormap.hpp
)
target_link_libraries(grav_sim_cpu PRIVATE
        raylib
        ${GLM_TARGET}
        TBB::tbb
)
target_include_directories(grav_sim_cpu PRIVATE
        include
)