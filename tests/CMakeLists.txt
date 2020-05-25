cmake_minimum_required(VERSION 3.8)

add_executable(
        test_curl-REST
        test_curl-REST.cpp
)
target_link_libraries(test_curl-REST PUBLIC curl-wrapper)

add_executable(
        test_utils
        test_utils.cpp
)
target_link_libraries(test_utils PUBLIC utils)

add_executable(
        test_REST-API
        test_REST-API.cpp
)
target_link_libraries(test_REST-API PUBLIC rucio-rest-api-wrapper globals)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
# Link gtest_utils with what we want to test and the GTest and pthread library
add_executable(
        gtest_utils 
        gtest_utils.cpp)

target_link_libraries(gtest_utils PUBLIC utils ${GTEST_LIBRARIES} pthread)

add_executable(
        gtest_curl-REST 
        gtest_curl-REST.cpp)

target_link_libraries(gtest_curl-REST PUBLIC curl-wrapper ${GTEST_LIBRARIES} pthread)