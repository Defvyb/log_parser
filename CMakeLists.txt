cmake_minimum_required(VERSION 3.8)

project(log_parser)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(rapidjson)
add_subdirectory(log_parser)
add_subdirectory(generator)

find_package(GTest)
if(GTest_FOUND)
    enable_testing()
    message("GTest found, tests will be compiled")
    add_subdirectory(tests)
endif()