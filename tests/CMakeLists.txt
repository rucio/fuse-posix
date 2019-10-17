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
target_link_libraries(test_REST-API PUBLIC rucio-rest-api-wrapper)