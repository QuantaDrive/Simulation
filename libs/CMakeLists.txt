### GLFW ###

add_subdirectory(glfw-3.4)
include_directories(glfw-3.4/include)

find_package(OpenGL REQUIRED)

### GLEW ###

add_definitions(
    -DGLEW_STATIC
)

set( GLEW_SOURCE
    glew-2.1.0/src/glew.c
)

set(GLEW_HEADERS glew-2.1.0/include/GL/glew.h)

add_library( GLEW_210 STATIC
    ${GLEW_SOURCE}
    ${GLEW_HEADERS}
)

target_link_libraries( GLEW_210
    ${OPENGL_LIBRARY}
    ${EXTRA_LIBS}
)

###

set( ALL_LIBS
    OpenGL::GL
    glfw
    GLEW_210
)