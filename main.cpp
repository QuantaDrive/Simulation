#include <Simulation/Simulation.h>
#include <yaml-cpp/yaml.h>
#include <fstream>

#include "src/DAL/IRepo.h"
#include "src/DAL/Repo.h"
#include "src/View/WindowManager.cpp"

namespace ed = ax::NodeEditor;
ed::EditorContext *g_Context = nullptr;
WindowManager windowManager = WindowManager();

int main()
{
	YAML::Node db = YAML::LoadFile("conf/db.yaml");
	Repo repo = Repo(db);
	repo.deleteArm("testarm");
	repo.updateArm("newarm","arm2","localhost");

	simulation::Init();
	simulation::CompileShaders();

	simulation::project();
	simulation::lookAt(
		glm::vec3(4,3,-3), // Camera is at (4,3,-3), in World Space
		glm::vec3(0,0,0)   // and looks at the origin
	);
	simulation::lightPosition(glm::vec3(4, 3, 3));

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

		model.rotate(glm::vec3(0, 1, 0), true, true); // Rotate the model by 1 degree around the y axis
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

