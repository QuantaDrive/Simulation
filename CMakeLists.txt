cmake_minimum_required(VERSION 3.30)
project(projectRobotArm)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        libs/glfw-3.4/include
        libs/glew-1.13.0/include
        libs/glm-0.9.6.3
        libs/imgui-1.91.8
        libs/imgui-node-editor
        libs/yaml-cpp/include
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

### yaml-cpp ###

add_subdirectory(libs/yaml-cpp)

###

add_subdirectory(src/Simulation)

add_executable(
        projectRobotArm
        main.cpp
        src/Domain/Instruction.cpp
        src/Domain/Instruction.h
        src/Domain/Task.cpp
        src/Domain/Task.h
        src/Domain/RobotArm.cpp
        src/Domain/RobotArm.h
        src/Domain/User.cpp
        src/Domain/User.h
        src/Physical/BL/PhysicalManager.cpp
        src/Physical/BL/PhysicalManager.h
        src/Simulation/BL/SimulationManager.cpp
        src/Simulation/BL/SimulationManager.h
        src/DAL/Repo.cpp
        src/DAL/Repo.h
        src/DAL/IRepo.h
        src/View/IWindowManager.h
        src/View/WindowManager.cpp
        src/Domain/Node.cpp
        src/Domain/Node.h
        src/Domain/NodeActivation.cpp
        src/Domain/NodeActivation.h
)

### ImGui Node Editor ###

add_library(ImGuiNodeEditor STATIC
        libs/imgui-node-editor/imgui_node_editor.h
        libs/imgui-node-editor/imgui_node_editor.cpp
        libs/imgui-node-editor/imgui_node_editor_api.cpp
        libs/imgui-node-editor/imgui_canvas.cpp
        libs/imgui-node-editor/examples/application/include/application.h
)

target_link_libraries(ImGuiNodeEditor
        ImGui
)

### Crude JSON ###

add_library(crude_json STATIC
        libs/imgui-node-editor/crude_json.h
        libs/imgui-node-editor/crude_json.cpp
)

### Executable ###

target_link_libraries(projectRobotArm
        OpenGL::GL
        glfw
        GLEW_1130
        glfw
        ImGui
        ImGuiNodeEditor
        crude_json
        Simulation
        yaml-cpp
)
