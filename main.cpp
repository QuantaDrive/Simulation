#include <Simulation/Simulation.h>

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

	do
	{
		simulation::refresh();

        model.render();
	} // Check if the ESC key was pressed or the window was closed
	while (simulation::needClose());

	simulation::Close();

	return 0;
}

