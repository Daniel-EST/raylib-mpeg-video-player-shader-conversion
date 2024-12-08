cmake_minimum_required(VERSION 3.22)
project(raylib-video-player-shader CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

##########################################################################################
# Add dependencies with FetchContent
##########################################################################################

function(add_git_dependency libName gitURL gitTag)
    FetchContent_Declare(${libName}
        GIT_REPOSITORY ${gitURL}
        GIT_TAG        ${gitTag}
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(${libName})
    target_compile_options(${libName} PRIVATE "-w")
endfunction()

# Add Raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
add_git_dependency(
    raylib
    https://github.com/raysan5/raylib.git
    5.5
)

# Add pl_mpeg header
file(DOWNLOAD
	https://raw.githubusercontent.com/phoboslab/pl_mpeg/refs/heads/master/pl_mpeg.h
	${CMAKE_BINARY_DIR}/external/include/pl_mpeg.h)

##########################################################################################
# Project executable setup
##########################################################################################

# Adding our source files
# Define PROJECT_SOURCES as a list of all source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/sources/*.cpp")

# Define PROJECT_INCLUDE to be the path to the include directory of the project
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/sources/")

# Declaring our executable
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/external/include")

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

target_compile_definitions(${PROJECT_NAME} PUBLIC SHADERS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders/")

add_definitions(-DCMAKE_BUILD_TYPE=Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -DRELEASE")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
