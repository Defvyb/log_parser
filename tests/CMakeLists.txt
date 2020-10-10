project(tests)

include_directories(${GTEST_INCLUDE_DIR})

add_executable(${PROJECT_NAME}
    main.cpp
    tests.cpp
)
include_directories(../speedometer/sources)

target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)

set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_STANDARD 17
            )

add_test(
    NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
)


