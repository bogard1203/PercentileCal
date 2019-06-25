cmake_minimum_required(VERSION 2.8.12)
project(percentile)

set(PROJECT_NAME percentile)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("include")
include_directories("Thirds")
add_executable(${PROJECT_NAME} src/main.cpp)

# Sophisticated test cases need to be crafted in order to test edge cases
include(CTest)
add_test ("1_to_200_test" ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/test/config_3_1.txt ${CMAKE_CURRENT_SOURCE_DIR}/test/unit_3 )
set_tests_properties ("1_to_200_test" PROPERTIES PASS_REGULAR_EXPRESSION "99% is 198")

add_test ("double_1_to_100_test" ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/test/config_3_2.txt ${CMAKE_CURRENT_SOURCE_DIR}/test/unit_3 )
set_tests_properties ("double_1_to_100_test" PROPERTIES PASS_REGULAR_EXPRESSION "99% is 99")

add_test ("random_test" ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/test/config_3_3.txt ${CMAKE_CURRENT_SOURCE_DIR}/test/unit_3 )
set_tests_properties ("random_test" PROPERTIES PASS_REGULAR_EXPRESSION "99% is 297")

