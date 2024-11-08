#include "common.cpp"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x_position, double y_position);

unsigned int window_width = 800, window_height = 800;
float delta_time = 0.0f, last_frame = 0.0f;
float mouse_last_x = (int)(window_width / 2), mouse_last_y = (int)(window_height / 2);
float camera_yaw = -90.0f;
float camera_pitch = 0.0f;
bool mouse_focused_window = true;

glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);


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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouse_callback);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader simple_shader("src/shaders/vertex_shader.glsl", "src/shaders/fragment_shader.glsl");

	float vertices[] = {
		// positions			// texture coords
		-0.5f, -0.5f, -0.5f, 	0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 	1.0f, 0.0f,
		 0.5f, 0.5f, -0.5f, 	1.0f, 1.0f,
		 0.5f, 0.5f, -0.5f, 	1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 	0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 	0.0f, 0.0f,
		 0.5f, -0.5f, 0.5f, 	1.0f, 0.0f,
		 0.5f, 0.5f, 0.5f, 		1.0f, 1.0f,
		 0.5f, 0.5f, 0.5f, 		1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 		0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 	0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, 		1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 	1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 	0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 	0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 		1.0f, 0.0f,

		 0.5f, 0.5f, 0.5f, 		1.0f, 0.0f,
		 0.5f, 0.5f, -0.5f, 	1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f, 	0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f, 	0.0f, 1.0f,
		 0.5f, -0.5f, 0.5f, 	0.0f, 0.0f,
		 0.5f, 0.5f, 0.5f, 		1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 	0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f, 	1.0f, 1.0f,
		 0.5f, -0.5f, 0.5f, 	1.0f, 0.0f,
		 0.5f, -0.5f, 0.5f, 	1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 	0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 	0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 	0.0f, 1.0f,
		 0.5f, 0.5f, -0.5f, 	1.0f, 1.0f,
		 0.5f, 0.5f, 0.5f, 		1.0f, 0.0f,
		 0.5f, 0.5f, 0.5f, 		1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 		0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 	0.0f, 1.0f
	};

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coordinates attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	/*
		You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	*/
	// glBindBuffer(GL_ARRAY_BUFFER, VAO);
	// glBindVertexArray(0);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);

	int width, height, nr_channels;
	unsigned char *data = stbi_load("src/images/COMP04_5.png", &width, &height, &nr_channels, 0);

	if(data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glm::vec3 global_up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 camera_direction;

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	while(!glfwWindowShouldClose(window))
	{
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

		unsigned int transform_location = glGetUniformLocation(simple_shader.ID, "transform");
		glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(trans));

		glm::mat4 model = glm::mat4(1.0f);
		// model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(1.0f, 0.5f, 0.2f));

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

		camera_direction.x = cos(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
		camera_direction.y = sin(glm::radians(camera_pitch));
		camera_direction.z = sin(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
		camera_front = glm::normalize(camera_direction);

		glm::mat4 camera_view;
		camera_view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);

		int model_location = glGetUniformLocation(simple_shader.ID, "model");
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));

		int view_location = glGetUniformLocation(simple_shader.ID, "camera_view");
		glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(camera_view));

		int projection_location = glGetUniformLocation(simple_shader.ID, "projection");
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

		simple_shader.use();
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// glDeleteProgram(shader_program);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	const float camera_speed = 2.5f * delta_time;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera_position += camera_speed * camera_front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera_position -= camera_speed * camera_front;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera_position += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera_position -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double x_position, double y_position)
{
	if(mouse_focused_window)
	{
		mouse_last_x = x_position;
		mouse_last_y = y_position;
		mouse_focused_window = false;
	}

	float x_offset = x_position - mouse_last_x;
	float y_offset = mouse_last_y - y_position;
	mouse_last_x = x_position;
	mouse_last_y = y_position;

	const float mouse_sensitivity = 0.1f;
	x_offset *= mouse_sensitivity;
	y_offset *= mouse_sensitivity;

	camera_yaw += x_offset;
	camera_pitch += y_offset;
	if(camera_pitch > 89.0f)
		camera_pitch = 89.0f;
	if(camera_pitch < -89.0f)
		camera_pitch = -89.0f;
}