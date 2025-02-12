cmake_minimum_required(VERSION 3.30)
project(simulatie)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    libs/glfw-3.4/include
    libs/glew-1.13.0/include
    libs/glm-0.9.6.3
)

### GLFW ###

add_subdirectory(libs/glfw-3.4)

find_package(OpenGL REQUIRED)

### GLEW ###

add_definitions(
    -DGLEW_STATIC
)

add_library( GLEW_1130 STATIC
    libs/glew-1.13.0/src/glew.c
    libs/glew-1.13.0/include/GL/glew.h
)

target_link_libraries( GLEW_1130
    ${OPENGL_LIBRARY}
    ${EXTRA_LIBS}
)

###

add_executable(simulatie
    main.cpp
    common/shader.cpp
    common/shader.hpp
)

target_link_libraries(simulatie
    OpenGL::GL
    glfw
    GLEW_1130
)
