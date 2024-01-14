set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/okami)

file(GLOB OGL_SOURCES ${SOURCES_DIR}/ogl/*.cpp)
file(GLOB OGL_HEADERS ${HEADER_DIR}/ogl/*.hpp)

file(GLOB GLFW_SOURCES ${SOURCES_DIR}/glfw/*.cpp)
file(GLOB GLFW_HEADERS ${HEADER_DIR}/glfw/*.hpp)

file(GLOB IM3D_SOURCES ${SOURCES_DIR}/im3d/*.cpp)
file(GLOB IM3D_HEADERS ${HEADER_DIR}/im3d/*.hpp)

file(GLOB MAIN_SOURCES ${SOURCES_DIR}/*.cpp)
file(GLOB MAIN_HEADERS ${HEADER_DIR}/*.hpp)

file(GLOB_RECURSE shader_SRC CONFIGURE_DEPENDS 
    "shaders/ogl/*.glsl")

message("Embedded Shaders: ${shader_SRC}")

# Embed OpenGL Shaders (LINUX)
add_custom_command(
	OUTPUT ogl_shader_rc.cpp
	COMMAND embedfile ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ogl ogl_shader_rc.cpp MakeGLShaderMap
	DEPENDS ${shader_SRC})

set(SOURCES ${MAIN_SOURCES} ${OGL_SOURCES} ${GLFW_SOURCES} ${IM3D_SOURCES} ogl_shader_rc.cpp)
set(HEADERS ${MAIN_HEADERS} ${OGL_HEADERS} ${GLFW_HEADERS} ${IM3D_HEADERS})

add_library(okami-core ${SOURCES} ${HEADERS})

target_include_directories(okami-core PUBLIC include)
target_include_directories(okami-core PRIVATE embed)

target_link_libraries(okami-core PUBLIC EnTT::EnTT)
target_link_libraries(okami-core PUBLIC glm::glm)

target_link_libraries(okami-core PRIVATE plog::plog)
target_link_libraries(okami-core PRIVATE glfw glad::glad)
target_link_libraries(okami-core PRIVATE unofficial::im3d::im3d)
target_link_libraries(okami-core PRIVATE assimp::assimp)
