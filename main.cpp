
#include <Simulation/Simulation.h>
#include "src/View/WindowManager.cpp"

namespace ed = ax::NodeEditor;
ed::EditorContext *g_Context = nullptr;
WindowManager windowManager = WindowManager();

int main()
{
	simulation::Init();
	simulation::CompileShaders();

	simulation::project();
	simulation::lookAt(
		glm::vec3(4,3,-3), // Camera is at (4,3,-3), in World Space
		glm::vec3(0,0,0)   // and looks at the origin
	);

	simulation::setBackgroundColor(0.0f, 0.0f, 0.4f);

	simulation::Mesh model = simulation::Mesh("monkey.obj");

	// Setup Node Editor
	ed::Config config;
	config.SettingsFile = "Simple.json";
	g_Context = ed::CreateEditor(&config);

	// Setup ImGui
	windowManager.SetupImGui(simulation::window);
	do
	{
		simulation::refresh();

        model.render();
		// Render ImGui UI and Node Editor together
		windowManager.RenderUI(g_Context);
	} // Check if the ESC key was pressed or the window was closed
	while (simulation::needClose());

	simulation::Close();
	// Cleanup ImGui & Node Editor
	windowManager.CleanupImGui(g_Context);

	return 0;
}

