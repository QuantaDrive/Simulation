#include <Simulation/Simulation.h>
#include <fstream>

#include <yaml-cpp/yaml.h>

#include "src/DAL/Repo.h"
#include "src/Domain/Position.h"
#include "src/Domain/RobotArm.h"
#include "src/Domain/Tool.h"
#include "src/Simulation/BL/SimulationManager.h"
#include "src/View/WindowManager.cpp"

namespace ed = ax::NodeEditor;
ed::EditorContext *g_Context = nullptr;
// WindowManager windowManager = WindowManager();

int main()
{
	Repo* repo= new Repo(YAML::LoadFile("conf/db.yaml"));
	auto testarm = repo->readArm("arm1");

	simulation::Init();
	simulation::CompileShaders();

	simulation::project();
	simulation::lookAt(
		glm::vec3(0,750/2,1000), // Camera is at (4,3,-3), in World Space
		glm::vec3(0,750/2,0)   // and looks at the origin
	);
	simulation::lightPosition(glm::vec3(0,750/2,1000));

	simulation::setBackgroundColor(0.86f, 0.86f, 0.86f);

	simulation::RobotArm* arm = new simulation::RobotArm("arm1", "src/Simulation/arms/Moveo/moveo.ini");

	vector<float> angles = {45.0f,-35.0f,90.0f,50.0f};
	SimulationManager* simulationManager= new SimulationManager(repo,arm);
	// angles = mgr->inverseKinematics(testarm,new domain::Position({0,0,0},{90,0,90}));
	// for (auto angle:angles)
	// {
	// 	cout << angle << endl;
	// }

	arm->moveAngle(1, angles[0] /*45.0f*/, false, true);
	arm->moveAngle(2, angles[1] /*-35.0f*/, false, true);
	arm->moveAngle(3, angles[2] /*90.0f*/, false, true);
	arm->moveAngle(5, angles[3] /*50.0f*/, false, true);

	// Setup Node Editor
	ed::Config config;
	config.SettingsFile = "Simple.json";
	g_Context = ed::CreateEditor(&config);

	// Setup ImGui
	WindowManager windowManager = WindowManager(simulationManager);
	windowManager.SetupImGui(simulation::window);
	do
	{
		simulation::refresh();

		arm->moveAngle(1, 0.2f, true, true);
		arm->moveAngle(4, 0.3f, true, true);
		arm->moveAngle(6, 1.0f, true, true);
		arm->render();
		// Render ImGui UI and Node Editor together
		windowManager.RenderUI(g_Context);
	} // Check if the ESC key was pressed or the window was closed
	while (simulation::needClose());

	simulation::Close();
	// Cleanup ImGui & Node Editor
	windowManager.CleanupImGui(g_Context);

	return 0;
}

