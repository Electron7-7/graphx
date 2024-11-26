#include "sanity.hpp"
#include "gl_window.hpp"
#include "gl_render.hpp"
#include "r_common.hpp"

DebugCamera debugging_camera(glm::vec3(0.0f, 0.0f, 3.0f));

float mouse_last[2];

void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double x_position_in, double y_position_in);

int main(int argc, char** argv)
{
	glfwInit();
	Window mainWindow = Window();

	glfwSetInputMode(mainWindow.w_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(mainWindow.w_window, mouseCallback);

	GLShader generic_shader("src/shaders/vertex_shader.glsl", "src/shaders/fragment_shader.glsl");
	Cube single_cube;

	unsigned int element_buffer, vertex_array, vertex_buffer;
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &element_buffer);
	glGenBuffers(1, &vertex_buffer);

	glBindVertexArray(vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

	single_cube.makeCube();

	glEnable(GL_DEPTH_TEST);

	mouse_last[0] = mainWindow.w_width / 2.0f;
	mouse_last[1] = mainWindow.w_height / 2.0f;

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	while(!glfwWindowShouldClose(mainWindow.w_window))
	{
		// mainWindow.SwapAndClear();
		processInput(mainWindow.w_window);
		glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 cube_location = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)mainWindow.w_width / (float)mainWindow.w_height, 0.1f, 100.0f);
		glm::mat4 camera_view = debugging_camera.getViewMatrix();

		generic_shader.setMatrix("model", cube_location);
		generic_shader.setMatrix("projection", projection);
		generic_shader.setMatrix("camera_view", camera_view);
		generic_shader.use();

		glBindVertexArray(vertex_array);
		single_cube.drawCube();

		glfwSwapBuffers(mainWindow.w_window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &element_buffer);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) ==  GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	int input_vector[2] =
	{
		glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S),
		glfwGetKey(window, GLFW_KEY_D) - glfwGetKey(window, GLFW_KEY_A)
	};

	debugging_camera.doMovement(input_vector);
}

void mouseCallback(GLFWwindow* window, double x_position_in, double y_position_in)
{
	float x_position = static_cast<float>(x_position_in), y_position = static_cast<float>(y_position_in);
	float mouse_offset[2] = { x_position - mouse_last[0], mouse_last[1] - y_position };

	mouse_last[0] = x_position;
	mouse_last[1] = y_position;

	debugging_camera.doMouseMovement(mouse_offset);
}