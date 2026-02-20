cmake_minimum_required(VERSION 3.16)

project(HawkPro
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(hawkpro ${SOURCES})

find_package(Curses REQUIRED)

target_include_directories(hawkpro PRIVATE ${CURSES_INCLUDE_DIR})
target_link_libraries(hawkpro PRIVATE ${CURSES_LIBRARIES})

