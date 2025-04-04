#ifndef IWINDOWMANAGER_H
#define IWINDOWMANAGER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui_node_editor.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include <vector>
#include "../Simulation/BL/SimulationManager.h"


class IWindowManager {
public:
   virtual ~IWindowManager() = default;

   virtual void setupImGui(GLFWwindow *existingWindow) = 0;
   virtual void renderUI(ax::NodeEditor::EditorContext *g_Context) = 0;
   virtual void cleanupImGui(ax::NodeEditor::EditorContext *g_Context) = 0;
};

#endif // IWINDOWMANAGER_H