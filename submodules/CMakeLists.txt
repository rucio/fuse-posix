include_directories(nlohmann_json/single_include)

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE nlohmann_json/single_include/nlohmann)

add_library(fastlog INTERFACE)
target_include_directories(fastlog INTERFACE fastlog)

include_directories(warpdrive/include warpdrive/include/interfaces)

add_library(warpdrive INTERFACE)
target_include_directories(warpdrive INTERFACE warpdrive)
