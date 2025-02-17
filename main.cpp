#include <Simulation/Simulation.h>

int main()
{
	simulation::Init(1024, 768);
	simulation::CompileShaders();

	simulation::project(45.0f, 4.0f / 3.0f);
	simulation::lookAt(
		glm::vec3(4,3,-3), // Camera is at (4,3,-3), in World Space
		glm::vec3(0,0,0)   // and looks at the origin
	);

	simulation::setBackgroundColor(0.0f, 0.0f, 0.4f);

	simulation::Mesh model = simulation::Mesh::loadObj("block.obj");

	do
	{
		simulation::refresh();

        model.render();

		simulation::showFrame();
	} // Check if the ESC key was pressed or the window was closed
	while (simulation::needClose());

	simulation::Close();

	return 0;
}

