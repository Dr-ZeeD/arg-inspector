cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
    arg-inspector
    VERSION 0.1.0
    DESCRIPTION "Inspect CLI arguments"
    LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
  URL_MD5 893b5203b862eb9bbd08553e24ff146a
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)

set(BOOST_INCLUDE_LIBRARIES process)
FetchContent_MakeAvailable(Boost)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP ON)
FetchContent_MakeAvailable(json)

add_executable(arg-inspector main.cpp)
target_link_libraries(arg-inspector PRIVATE Boost::process nlohmann_json::nlohmann_json)