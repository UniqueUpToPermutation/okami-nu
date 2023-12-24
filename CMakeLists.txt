cmake_minimum_required(VERSION 3.0.0)
project(okami VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(expected-lite CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

include(CTest)
enable_testing()

add_subdirectory(core)
add_subdirectory(tests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
