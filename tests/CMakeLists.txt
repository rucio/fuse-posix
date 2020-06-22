add_executable(
        test_curl-REST
        test_curl-REST.cpp)
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

add_executable(
        test_rucio_download_info
        test_rucio_download_info.cpp
)
target_link_libraries(test_rucio_download_info PUBLIC rucio-fuse globals)

add_executable(
        test_x509_auth
        test_x509_auth.cpp
)
target_link_libraries(test_x509_auth PUBLIC rucio-rest-api-wrapper globals)