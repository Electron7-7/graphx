#include <g_common.hpp>
#include <g_cube.cpp>
#include <g_shader.cpp>
#include <g_camera.cpp>

Camera player_camera(glm::vec3(0.0f, 0.0f, 3.0f));

unsigned int window_width = 1280, window_height = 720;
float mouse_last_x = (int)(window_width / 2), mouse_last_y = (int)(window_height / 2);
float delta_time = 0.0f, last_frame = 0.0f;

void processInput(GLFWwindow* window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double x_position, double y_position);

int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Fucking Graphics", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowPos(window, (int)((1920 - window_width) / 2), (int)((1080 - window_height) /2));
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader simple_shader("src/shaders/vertex_shader.glsl", "src/shaders/fragment_shader.glsl");
	Cube new_cube;

	unsigned int element_buffer, vertex_array, vertex_buffer;
	glGenBuffers(1, &element_buffer);
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &vertex_buffer);

	glBindVertexArray(vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

	new_cube.makeCube();

	glEnable(GL_DEPTH_TEST);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Wireframe mode

	//
	// Runtime Loop
	//
	while(!glfwWindowShouldClose(window))
	{
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		processInput(window);
		glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model_location = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);
		glm::mat4 camera_view = player_camera.GetViewMatrix();

		simple_shader.setMatrix("model", model_location);
		simple_shader.setMatrix("projection", projection);
		simple_shader.setMatrix("camera_view", camera_view);
		simple_shader.use();

		glBindVertexArray(vertex_array);
		new_cube.drawCube();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &element_buffer);
	glDeleteProgram(simple_shader.ID);
	glfwTerminate();
	return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	window_width = width;
	window_height = height;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	int input_vector[2] =
	{
		glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S),
		glfwGetKey(window, GLFW_KEY_D) - glfwGetKey(window, GLFW_KEY_A)
	};

	player_camera.DoMovement(input_vector, delta_time);
}

void mouseCallback(GLFWwindow* window, double x_pos_in, double y_pos_in)
{

	float x_position = static_cast<float>(x_pos_in), y_position = static_cast<float>(y_pos_in);
	float x_offset = x_position - mouse_last_x, y_offset = mouse_last_y - y_position;

	mouse_last_x = x_position;
	mouse_last_y = y_position;

	player_camera.ProcessMouseMovement(x_offset, y_offset);
}