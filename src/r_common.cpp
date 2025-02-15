#include "r_common.hpp"
#include "g_common.hpp"
#include "t_common.hpp"
#include "quad.graphxmodel"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace graphx;

//
// GLShader
//
GLShader::GLShader(std::filesystem::path vertex_shader_path, std::filesystem::path fragment_shader_path)
{
	std::string vertex_code;
	std::string fragment_code;
	std::ifstream v_shader_file;
	std::ifstream f_shader_file;

	v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		v_shader_file.open(vertex_shader_path);
		f_shader_file.open(fragment_shader_path);
		std::stringstream v_shader_stream, f_shader_stream;

		v_shader_stream << v_shader_file.rdbuf();
		f_shader_stream << f_shader_file.rdbuf();

		v_shader_file.close();
		f_shader_file.close();

		vertex_code = v_shader_stream.str();
		fragment_code = f_shader_stream.str();
	}

	catch(std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	buildShader(vertex_code, fragment_code);
};

GLShader::GLShader(std::string vertex_shader_code, std::string fragment_shader_code)
{
	buildShader(vertex_shader_code, fragment_shader_code);
}

void GLShader::buildShader(std::string vertex_shader_string, std::string fragment_shader_string)
{
	const char *v_shader_code = vertex_shader_string.c_str();
	const char *f_shader_code = fragment_shader_string.c_str();

	unsigned int vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v_shader_code, NULL);
	glCompileShader(vertex);

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f_shader_code, NULL);
	glCompileShader(fragment);

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

template<> void GLShader::setUniform<bool>(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	// int new_value;
	// glGetUniformiv(id, glGetUniformLocation(id, name.c_str()), &new_value);
	// return (bool)new_value;
}

template<> void GLShader::setUniform<int>(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	// int new_value;
	// glGetUniformiv(id, glGetUniformLocation(id, name.c_str()), &new_value);
	// return new_value;
}

template<> void GLShader::setUniform<float>(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	// float new_value;
	// glGetUniformfv(id, glGetUniformLocation(id, name.c_str()), &new_value);
	// return new_value;
}

template<> void GLShader::setUniform<glm::vec2>(const std::string &name, glm::vec2 value) const
{
	glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
	// float *new_value = NULL;
	// glGetUniformfv(id, glGetUniformLocation(id, name.c_str()), new_value);
	// return glm::vec2(new_value[0], new_value[1]);
}

template<> void GLShader::setUniform<glm::vec3>(const std::string &name, glm::vec3 value) const
{
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
	// float *new_value = NULL;
	// glGetUniformfv(id, glGetUniformLocation(id, name.c_str()), new_value);
	// return glm::vec3(new_value[0], new_value[1], new_value[2]);
}

template<> void GLShader::setUniform<glm::vec4>(const std::string &name, glm::vec4 value) const
{
	glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
	// float *new_value = NULL;
	// glGetUniformfv(id, glGetUniformLocation(id, name.c_str()), new_value);
	// return glm::vec4(new_value[0], new_value[1], new_value[2], new_value[3]);
}

template<> void GLShader::setUniform<glm::mat3>(const std::string &name, glm::mat3 value) const
{
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	// return glm::mat3(0.0f); // I don't know how to get and return Matrices properly, but I don't want a non-void function to not return something
}

template<> void GLShader::setUniform<glm::mat4>(const std::string &name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	// return glm::mat4(0.0f); // I don't know how to get and return Matrices properly, but I don't want a non-void function to not return something
}

//
// Window Functions
//
GLFWwindow *W_CreateWindow(int width, int height, const char *title, bool make_context_current)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *new_window = glfwCreateWindow(width, height, title, NULL, NULL);
	
	if(new_window == NULL)
	{
		std::cerr << "[ERROR] Failed to create GLFW window!" << std::endl;
		glfwTerminate();
	}

	if(make_context_current)
		glfwMakeContextCurrent(new_window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cerr << "[ERROR] Failed to initialize GLAD!" << std::endl;

	return new_window;
}

//
// Device
//
void Device::loadSettings(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;

	setRawData(name, new_settings["Name"]);
}

void Device::initialize(Theatre *parent_theatre)
{}

void Device::prepForDestruction()
{}

void Device::setUID(long manual_uid)
{
	if(manual_uid != -1)
		UID = manual_uid;
}

long Device::getUID()
{
	return UID;
}

//
// Environment
//
glm::vec3 Environment::getAmbientLight()
{
	return ambient_light_color * ambient_light_strength * (int)ambient_lighting_enabled;
}

//
// Environment
//
Environment::Environment(bool enable_ambient_lighting, glm::vec3 init_ambient_color, float init_ambient_strength)
: ambient_lighting_enabled(enable_ambient_lighting), ambient_light_color(init_ambient_color), ambient_light_strength(init_ambient_strength)
{
	device_type = DEVICE_ENVIRONMENT;
}

void Environment::loadSettings(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;
	Device::loadSettings(new_settings);

	setRawData(ambient_lighting_enabled, new_settings["AmbientLightingEnabled"]);
	setRawData(ambient_light_color, new_settings["AmbientLightingColor"]);
	setRawData(ambient_light_strength, new_settings["AmbientLightingStrength"]);
}

//
// Material
//
Material::Material(bool is_fullbright, glm::vec3 init_color)
: embedded_texture_diffuse(NO_TEXTURE), color(init_color), specular_strength(0.0f), mat_fullbright(is_fullbright)
{
	device_type = DEVICE_MATERIAL;
}

Material::Material(unsigned char *init_diffuse_texture, unsigned char *init_specular_texture, int init_specular_sharpness, float init_specular_strength, glm::vec3 init_color)
: embedded_texture_diffuse(init_diffuse_texture), embedded_texture_specular(init_specular_texture), color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength), mat_fullbright(false)
{
	device_type = DEVICE_MATERIAL;
}

Material::Material(glm::vec3 init_color, float init_specular_strength, unsigned int init_specular_sharpness)
: color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength), mat_fullbright(false)
{
	device_type = DEVICE_MATERIAL;
}

void Material::loadSettings(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;
	Device::loadSettings(new_settings);

	setVariable(embedded_texture_diffuse, new_settings["DiffuseTexture"]);
	setVariable(embedded_texture_specular, new_settings["SpecularTexture"]);
	setRawData(color, new_settings["Color"]);
	setRawData(specular_sharpness, new_settings["SpecularSharpness"]);
	setRawData(specular_strength, new_settings["SpecularStrength"]);
	setRawData(mat_fullbright, new_settings["mat_fullbright"]);
}

unsigned int Material::bufferTextureFromMemory(unsigned char *texture_buffer)
{
	stbi_set_flip_vertically_on_load(true); // Obviously, automate this to flip relevant textures (when Y-Axis 0.0 is not on the bottom of the image)

	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int t_width, t_height, t_channels;
	unsigned char *t_data = stbi_load_from_memory(texture_buffer, 64*64, &t_width, &t_height, &t_channels, STBI_rgb);

	if(!t_data)
	{
		std::cerr << "Failed to load texture!" << std::endl;
		return 20;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, t_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(t_data);

	return texture_id;
}

//
// Mesh
//
Mesh::Mesh(Material *init_material, std::vector<GLfloat> init_vertices, std::vector<GLuint> init_indices, int init_vao_index, std::string init_name)
: name(init_name), material(init_material), vao_index(init_vao_index), vertices(init_vertices), indices(init_indices)
{
	device_type = DEVICE_MESH;
}

void Mesh::loadSettings(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;
	Device::loadSettings(new_settings);

	gMeshData mesh_data;

	setRawData(name, new_settings["Name"]);
	setDevicePointer(material, new_settings["Material"]);
	setVariable(mesh_data, new_settings["MeshData"]);

	vertices = std::get<0>(mesh_data);
	indices = std::get<1>(mesh_data);
	vao_index = std::get<2>(mesh_data);
}

//
// Sprite
//
Sprite::Sprite(Material *init_material, int init_vao_index)
: Mesh(init_material, QUAD_VERTS, QUAD_INDICES, init_vao_index)
{
	device_type = DEVICE_SPRITE;
}

void Sprite::loadSettings(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;
	Mesh::loadSettings(new_settings);
}
