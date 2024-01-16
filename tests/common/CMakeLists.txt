set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/okami/test)

file(GLOB TEST_SOURCES ${SOURCES_DIR}/*.cpp)
file(GLOB TEST_HEADERS ${HEADER_DIR}/*.hpp)

set(SOURCES ${TEST_SOURCES})
set(HEADERS ${TEST_HEADERS})

add_library(okami-test-common ${SOURCES} ${HEADERS})

target_link_libraries(okami-test-common PUBLIC okami-core)
target_link_libraries(okami-test-common PUBLIC plog::plog)
target_link_libraries(okami-test-common PUBLIC glfw glad::glad)
target_link_libraries(okami-test-common PUBLIC unofficial::im3d::im3d)

target_include_directories(okami-test-common PUBLIC include)