cmake_minimum_required(VERSION 4.1)
project(grav_sim_cpu)
set(CMAKE_CXX_STANDARD 26)

include(FetchContent)

find_package(raylib)

find_package(glm CONFIG QUIET)
if(NOT glm_FOUND AND GAL_FETCH_DEPENDENCIES)
    message(STATUS "Fetching GLM...")
    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG 1.0.3
    )
    FetchContent_MakeAvailable(glm)
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
        include/config.hpp)
target_link_libraries(grav_sim_cpu PRIVATE
    raylib
    ${GLM_TARGET}
)
target_include_directories(grav_sim_cpu PRIVATE
    include
)