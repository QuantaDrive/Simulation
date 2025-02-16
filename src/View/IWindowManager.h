#ifndef IWINDOWMANAGER_H
#define IWINDOWMANAGER_H

#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
class IWindowManager {
public:
   virtual void init(GLFWwindow* existingWindow) = 0;
   virtual void run() = 0;
   virtual void close() = 0;
   virtual ~IWindowManager() = default;
};

#endif // IWINDOWMANAGER_H
