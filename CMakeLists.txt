cmake_minimum_required(VERSION 3.30)
project(simulatie)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        libs/glfw-3.4/include
        libs/glew-1.13.0/include
        libs/glm-0.9.6.3
        libs/imgui-1.91.8
)

### GLFW ###

add_subdirectory(libs/glfw-3.4)

find_package(OpenGL REQUIRED)

### GLEW ###

add_definitions(
        -DGLEW_STATIC
)

add_library(GLEW_1130 STATIC
        libs/glew-1.13.0/src/glew.c
        libs/glew-1.13.0/include/GL/glew.h
)

target_link_libraries(GLEW_1130
        ${OPENGL_LIBRARY}
        ${EXTRA_LIBS}
)

### ImGui ###

add_library(ImGui STATIC
        libs/imgui-1.91.8/imstb_rectpack.h
        libs/imgui-1.91.8/imstb_textedit.h
        libs/imgui-1.91.8/imstb_truetype.h
        libs/imgui-1.91.8/imgui_internal.h
        libs/imgui-1.91.8/imgui.h
        libs/imgui-1.91.8/imconfig.h
        libs/imgui-1.91.8/imgui.cpp
        libs/imgui-1.91.8/imgui_draw.cpp
        libs/imgui-1.91.8/imgui_widgets.cpp
        libs/imgui-1.91.8/imgui_tables.cpp
        libs/imgui-1.91.8/imgui_demo.cpp
        libs/imgui-1.91.8/backends/imgui_impl_glfw.h
        libs/imgui-1.91.8/backends/imgui_impl_opengl3.h
        libs/imgui-1.91.8/backends/imgui_impl_opengl3.cpp
        libs/imgui-1.91.8/backends/imgui_impl_glfw.cpp
        libs/imgui-1.91.8/misc/cpp/imgui_stdlib.h
        libs/imgui-1.91.8/misc/cpp/imgui_stdlib.cpp
)

###

add_executable(
        simulatie
        main.cpp
        common/shader.cpp
        common/shader.hpp
        src/Simulation/SimulationInit.cpp
        src/Simulation/SimulationInit.h
        src/Domain/ArmPosition.cpp
        src/Domain/ArmPosition.h
        src/Domain/Task.cpp
        src/Domain/Task.h
        src/Domain/RobotArm.cpp
        src/Domain/RobotArm.h
        src/Domain/User.cpp
        src/Domain/User.h
)

target_link_libraries(simulatie
        OpenGL::GL
        glfw
        GLEW_1130
        ImGui
)
