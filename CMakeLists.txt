set(PROJECT_NAME Fifteen)

cmake_minimum_required(VERSION 3.1)

project(${PROJECT_NAME})

# General Source tree files.
file(GLOB HEADERFILES "src/*.h")
file(GLOB SOURCEFILES "src/*.c")

#glfw
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/include)
FILE( GLOB GLFW ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/glfw3.lib)

#glad
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include/)
FILE( GLOB GLAD_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/src/glad.c)

#SOIL
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/soil/src/)
FILE( GLOB SOIL ${CMAKE_CURRENT_SOURCE_DIR}/libs/soil/SOIL.lib)

#cglm
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/cglm/include/)
FILE( GLOB CGLM ${CMAKE_CURRENT_SOURCE_DIR}/libs/cglm/cglm.lib)

# Finding packages.
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${HEADERFILES} ${SOURCEFILES} ${GLAD_SOURCE})
target_link_libraries(${PROJECT_NAME} ${GLFW} ${OPENGL_LIBRARIES} ${SOIL} ${CGLM})
