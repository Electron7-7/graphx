// r_main.hpp - rendering code specifically for the renderer logic
#include "sanity.hpp"
#include <vector>

#define GLSHADER_TYPE_VERTEX 0
#define GLSHADER_TYPE_FRAGMENT 1
#define GLSHADER_TYPE_PROGRAM 2

#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING

class GLShader
{
public:
	unsigned int ID;

	GLShader(const char* vertex_path, const char* fragment_path);

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMatrix(const std::string& name, glm::mat4 value) const;

private:
	void shaderErrorHandler(int thing, int type);
};

struct RenderStorageCmd
{
	GLuint VAO, VBO, EBO;
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	RenderStorageCmd(std::vector<GLfloat> new_vertices, std::vector<GLuint> new_indices) : vertices(new_vertices), indices(new_indices)
	{}
};

extern std::vector<GLuint> render_buffer_storage;
extern std::vector<GLuint> render_indices_amount_storage;
extern std::vector<RenderStorageCmd> render_storage_commands;

// extern u_int16_t main_window_size[2];
// extern float mouse_last[2];

GLFWwindow* W_CreateWindow(u_int16_t width, u_int16_t height, const char* title = "Fucking GraphX", bool make_context_current = true);
void W_SwapAndClear(GLFWwindow* w_window, float clear_color_r = 0.3f, float clear_color_g = 0.4f, float clear_color_b = 0.7f, float clear_color_a = 1.0f);
void R_StoreBuffers();
void R_Render(GLShader current_shader);
#endif

#ifndef GRAPHX_RENDERING_COMMON
#define GRAPHX_RENDERING_COMMON

struct Vertex
{
	glm::vec3 v_position;
	glm::vec2 v_texture_coordinate;

	Vertex(glm::vec3 position, glm::vec2 texture_coordinate) : v_position(position), v_texture_coordinate(texture_coordinate)
	{}

	inline glm::vec3 getPosition() { return v_position; }
	inline glm::vec2 getTextureCoordinate() { return v_texture_coordinate; }
};

struct Mesh
{
	const char* texture_image;

	Mesh(std::vector<float> new_vertices, std::vector<unsigned int> new_indices);

	bool isEmpty();
};

GLuint T_GenerateTexture(const char* filepath);
#endif