#include "sanity.hpp"
#include <models.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#define TINYOBJLOADER_DONOT_INCLUDE_MAPBOX_EARCUT
#include <earcut.hpp>
#include <tiny_obj_loader.h>

namespace TO = tinyobj;

std::string VERTEX_SHADER = R"~(
#version 460 core
layout (location = 0) in vec3 _vertex_position;
layout (location = 1) in vec3 _vertex_normal;
layout (location = 2) in vec2 _vertex_texture_coordinate;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 fragment_position;

void main()
{
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(_vertex_position, 1.0f);
	fragment_position = vec3(model_matrix * vec4(_vertex_position, 1.0f)); 
};
)~";

std::string FRAGMENT_SHADER = R"~(
#version 460 core
out vec4 FragColor;

in vec3 fragment_position;

void main()
{
	FragColor = vec4((1.0f + fragment_position) / 2.0f, 1.0f);
};
)~";

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *main_window = glfwCreateWindow(720.0f, 720.0f, "OBJ Importer Test", NULL, NULL);
	glfwMakeContextCurrent(main_window);

	if(main_window == nullptr)
	{
		std::cerr << "[ERROR] Failed to create GLFW window!" << std::endl;
		glfwTerminate();
	}

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cerr << "[ERROR] Failed to initialize GLAD!" << std::endl;
	
	const GLFWvidmode *primary_monitor_video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int primary_monitor_xposition = 0;
	int primary_monitor_yposition = 0;
	glfwGetMonitorPos(glfwGetPrimaryMonitor(), &primary_monitor_xposition, &primary_monitor_yposition);
	glfwSetWindowPos(main_window, static_cast<int>(((primary_monitor_video_mode->width - 720.0f) / 2) + primary_monitor_xposition), static_cast<int>(((primary_monitor_video_mode->height - 720.0f) / 2) + primary_monitor_yposition));
	glfwSetKeyCallback(main_window, keyCallback);

	std::vector<glm::vec3> obj_vertices;
	std::vector<glm::vec3> obj_indices;

	unsigned int vertex_shader;
	unsigned int fragment_shader;
	unsigned int shader_program;

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	const char *vertex_shader_string = VERTEX_SHADER.c_str();
	const char *fragment_shader_string = FRAGMENT_SHADER.c_str();

	glShaderSource(vertex_shader, 1, &vertex_shader_string, nullptr);
	glShaderSource(fragment_shader, 1, &fragment_shader_string, nullptr);

	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	TO::ObjReaderConfig reader_config;
	TO::ObjReader reader;

	std::string inputfile = "src/include/ERROR_Mesh.obj";

	if(!reader.ParseFromFile(inputfile, reader_config))
	{
		if(!reader.Error().empty())
		{
			std::cerr << "TinyObjReader: " << reader.Error();
		}

		exit(1);
	}

	if (!reader.Warning().empty())
	{
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();

	// std::vector<float> vertices = CUBE_VERTS;
	// std::vector<unsigned int> indices = CUBE_INDICES;
	std::vector<float> vertices;
	std::vector<int> indices;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++)
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
				tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
				tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

				std::cout << "Vertex: " << vx << ", " << vy << ", " << vz << std::endl;
				vertices.insert(vertices.end(), {vx, vy, vz});

				// Check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0)
				{
					tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
					tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
					tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];

					std::cout << "Normal Index: " << nx << ", " << ny << ", " << nz << std::endl;
					vertices.insert(vertices.end(), {nx, ny, nz});
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0)
				{
					tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
					tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
				
					std::cout << "Texture Coordinate Index: " << tx << ", " << ty << std::endl;
					vertices.insert(vertices.end(), {tx, ty});
				}
			}

			index_offset += fv;
		}
	}

	int it = 1;
	for(int i = 0 ; i < vertices.size() ; i+=3,it++)
	{
		std::cout << vertices[i] << ", " << vertices[i+1] << ", " << vertices[i+2]/* << ",\t\t" << vertices[i+3] << ", " << vertices[i+4] << ", " << vertices[i+5] << ",\t\t" << vertices[i+6] << ", " << vertices[i+7] */<< "\t\tVertex #" << it << std::endl;
	}

	it = 1;
	for(int i = 0 ; i < indices.size() ; i+=3,it++)
	{
		std::cout << indices[i] << ", " << indices[i+1] << ", " << indices[i+2] << "\t\tTriangle #" << it << std::endl;
	}

	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::float32_t), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	glEnable(GL_DEPTH_TEST);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode

	while(!glfwWindowShouldClose(main_window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		glUseProgram(shader_program);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glm::mat4 view_matrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection_matrix = glm::mat4(1.0f);
        view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, 0.0f, -2.0f));
		projection_matrix = glm::perspective(glm::radians(45.0f), (float)720.0f / (float)720.0f, 0.01f, 100.0f);

		glm::mat4 model_matrix = glm::mat4(1.0f);
		model_matrix = glm::translate(model_matrix, glm::vec3(-0.5f, 0.0f, 0.0f));
		model_matrix = glm::scale(model_matrix, glm::vec3(1.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model_matrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glfwSwapBuffers(main_window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}