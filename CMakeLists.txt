cmake_minimum_required(VERSION 3.0)
project(apear)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "src/*.cpp")
message(${SOURCES})

add_library(apear SHARED ${SOURCES})
target_include_directories(apear PUBLIC
    "include"
    "/usr/include/eigen3"
)
# target_link_libraries(apear Boost::serialization Boost::filesystem)
