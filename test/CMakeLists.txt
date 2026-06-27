cmake_minimum_required(VERSION 4.0)
project(EngineTest LANGUAGES CXX)

add_executable(${PROJECT_NAME} test_main.cc)

target_link_libraries(${PROJECT_NAME} PRIVATE Engine vendor)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})