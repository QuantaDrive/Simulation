#ifndef IWINDOWMANAGER_H
#define IWINDOWMANAGER_H

#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_node_editor.h>
class IWindowManager {
public:
   virtual void Init(GLFWwindow* existingWindow) = 0;
   virtual void Run() = 0;
   virtual void Close() = 0;
   virtual ~IWindowManager() = default;
};

#endif // IWINDOWMANAGER_H
