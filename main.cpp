#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <Simulation/Simulation.h>

int main()
{
	simulation::Init();
	simulation::CompileShaders();

	simulation::setBackgroundColor(0.0f, 1.0f, 0.0f);

	Mesh monkey = Mesh::loadObj("monkey.obj");

	do
	{
		simulation::refresh();

        monkey.render();
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(simulation::window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(simulation::window) == 0);

	simulation::Close();

	return 0;
}

