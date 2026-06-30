cmake_minimum_required(VERSION 4.0)
project(EngineTest LANGUAGES CXX)

add_library(test_vendor INTERFACE)
target_link_libraries(test_vendor INTERFACE GTest::gtest_main)

add_executable(${PROJECT_NAME} test_main.cc)

target_link_libraries(${PROJECT_NAME} PRIVATE Engine test_vendor)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})