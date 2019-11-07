include_directories(SHA1)

add_library(sha1
        SHA1/sha1.hpp
        SHA1/tests.cpp)

include_directories(nlohmann_json/single_include)

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE nlohmann_json/single_include/nlohmann)

add_library(fastlog INTERFACE)
target_include_directories(fastlog INTERFACE fastlog)
