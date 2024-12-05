#define STB_IMAGE_IMPLEMENTATION
#include "sanity.hpp"
#include "r_main.hpp"
#include "r_common.hpp"
#include "g_actors.hpp"
#include "state.hpp"

GraphXPlayer player("Player", glm::vec3(0.0f, 0.0f, 3.0f));

u_int16_t main_window_size[2] = { 1280, 720 };
float mouse_last[2];

void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in);


int main(int argc, char* *argv)
{
	glfwInit();
	GLFWwindow *main_window = W_CreateWindow(main_window_size[0], main_window_size[1]);
	glfwSetWindowPos(main_window, static_cast<int>((1920 - main_window_size[0]) / 2), static_cast<int>((1080 - main_window_size[1]) / 2)); // HARDCODED NATIVE RESOLUTION!!! CHANGE THIS!!!

	glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(main_window, mouseCallback);

	GLShader generic_shader("src/shaders/default_vertex_shader.glsl", "src/shaders/default_fragment_shader.glsl");

	unsigned int test_indices[36] =
	{
		// -X Square
		0, 1, 2,	// Bottom Triangle
		3, 1, 2,	// Top Triangle

		// +X Square
		4, 5, 6,
		7, 5, 6,

		// -Y Square
		0, 1, 4,
		5, 1, 4,

		// +Y Square
		2, 3, 6,
		7, 3, 6,

		// -Z Square
		0, 2, 4,
		6, 2, 4,

		// +Z Square
		1, 3, 5,
		7, 3, 5
	};

	float v_pos[3] = {-1.0f, -1.0f, -1.0f};
	float v_coord[2] = {0.0f, 0.0f};
	Vertex vertex_one(*v_pos, *v_coord);
	float v_pos2[3] = {-1.0f, -1.0f,  1.0f};
	float v_coord2[2] = {1.0f, 0.0f};
	Vertex vertex_two(*v_pos2, *v_coord2);
	float v_pos3[3] = {-1.0f,  1.0f, -1.0f};
	float v_coord3[2] = {0.0f, 1.0f};
	Vertex vertex_three(*v_pos3, *v_coord3);
	float v_pos4[3] = {-1.0f, 1.0f, 1.0f};
	float v_coord4[2] = {1.0f, 1.0f};
	Vertex vertex_four(*v_pos4, *v_coord4);
	float v_pos5[3] = {1.0f, -1.0f, -1.0f};
	float v_coord5[2] = {1.0f, 0.0f};
	Vertex vertex_five(*v_pos5, *v_coord5);
	float v_pos6[3] = {1.0f, -1.0f,  1.0f};
	float v_coord6[2] = {0.0f, 0.0f};
	Vertex vertex_six(*v_pos6, *v_coord6);
	float v_pos7[3] = {1.0f,  1.0f, -1.0f};
	float v_coord7[2] = {1.0f, 1.0f};
	Vertex vertex_seven(*v_pos7, *v_coord7);
	float v_pos8[3] = {1.0f,  1.0f,  1.0f};
	float v_coord8[2] = {0.0f, 1.0f};
	Vertex vertex_eight(*v_pos8, *v_coord8);

	std::vector<Vertex> cube_verts =
	{
		vertex_one,
		vertex_two,
		vertex_three,
		vertex_four,
		vertex_five,
		vertex_six,
		vertex_seven,
		vertex_eight
	};

	Mesh test_mesh(cube_verts, test_indices);

	glEnable(GL_DEPTH_TEST);

	mouse_last[0] = main_window_size[0] / 2.0f;
	mouse_last[1] = main_window_size[1] / 2.0f;

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	while(!glfwWindowShouldClose(main_window))
	{
		W_SwapAndClear(main_window);
		processInput(main_window);

		glm::mat4 tester_location = glm::mat4(1.0f);
		// tester_location = glm::translate(tester_location, tester.position_global);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)main_window_size[0] / (float)main_window_size[1], 0.1f, 100.0f);
		glm::mat4 camera_view = player.getViewMatrix();

		generic_shader.setMatrix("model", tester_location);
		generic_shader.setMatrix("projection", projection);
		generic_shader.setMatrix("camera_view", camera_view);
		generic_shader.use();

		test_mesh.draw();

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) ==  GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	int input_vector[2] =
	{
		glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S),
		glfwGetKey(window, GLFW_KEY_D) - glfwGetKey(window, GLFW_KEY_A)
	};

	player.doMovement(input_vector);
}

void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in)
{
	float x_position = static_cast<float>(x_position_in), y_position = static_cast<float>(y_position_in);
	float mouse_offset[2] = { x_position - mouse_last[0], mouse_last[1] - y_position };

	mouse_last[0] = x_position;
	mouse_last[1] = y_position;

	player.doMouseMovement(mouse_offset);
}