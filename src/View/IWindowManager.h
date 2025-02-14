//
// Created by ComicSansEnfants on 14/02/2025.
//

#ifndef IWINDOWMANAGER_H
#define IWINDOWMANAGER_H
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"


class IWindowManager {
public:
   bool init();
   void run();
   void close();
};



#endif //IWINDOWMANAGER_H
