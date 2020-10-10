cmake_minimum_required(VERSION 3.8)
project(log_parser)

file(GLOB_RECURSE headers_hpp ${PROJECT_SOURCE_DIR}/sources/*.hpp)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/sources/*.h)
file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/sources/*.cpp)
include_directories(${PROJECT_SOURCE_DIR}/sources)

add_executable(${PROJECT_NAME} ${headers} ${sources} ${headers_hpp})

set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_STANDARD 17
            )

set(CMAKE_CXX_FLAGS "-Ofast")
