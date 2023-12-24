# Set the directory containing your source files
set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Get all .cpp files in the source directory
file(GLOB SOURCES ${SOURCES_DIR}/*.cpp)

add_library(okami-core ${SOURCES})

target_include_directories(okami-core PUBLIC include)

target_link_libraries(okami-core PUBLIC EnTT::EnTT)
target_link_libraries(okami-core PRIVATE glfw glad::glad)