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