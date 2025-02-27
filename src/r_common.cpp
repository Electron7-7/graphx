#include "r_common.hpp"
#include "t_settings.hpp"
#include "g_jolt.hpp"
#include "quad.graphxmodel"
#include <iostream>

// Forward Declarations

using namespace graphx;
using namespace graphx::classes;

std::map<int, Device*(*)()> device_map =
{
	{graphx::classes::ENVIRONMENT, &createNewDevice<Environment>},
	{graphx::classes::MATERIAL, &createNewDevice<Material>},
	{graphx::classes::MESH, &createNewDevice<Mesh>},
	{graphx::classes::SPRITE, &createNewDevice<Sprite>},
	{graphx::classes::COLLIDER, &createNewDevice<Collider>},
};

//
// GLShader
//
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
}

template<> void GLShader::setUniform<int>(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

template<> void GLShader::setUniform<float>(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

template<> void GLShader::setUniform<glm::vec2>(const std::string &name, glm::vec2 value) const
{
	glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

template<> void GLShader::setUniform<glm::vec3>(const std::string &name, glm::vec3 value) const
{
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

template<> void GLShader::setUniform<glm::vec4>(const std::string &name, glm::vec4 value) const
{
	glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

template<> void GLShader::setUniform<glm::mat3>(const std::string &name, glm::mat3 value) const
{
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

template<> void GLShader::setUniform<glm::mat4>(const std::string &name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
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
Device::Device()
{
	my_type = graphx::classes::DEVICE;
}

bool Device::isType(int class_type)
{
	return class_type == my_type;
}

std::string Device::getTypeName()
{
	return graphx::classnames.at(my_type);
}

long Device::getType()
{
	return my_type;
}

void Device::setName(std::string new_name)
{
	name = new_name;
}

void Device::setName(char *new_name)
{
	name = new_name;
}

std::string Device::getName()
{
	return name;
}

void Device::loadSettings(graphx::gSettings new_settings)
{
	if(settings.contains(empty_settings_identifier))
		settings = new_settings;
	if(new_settings.contains(empty_settings_identifier))
		new_settings = settings;

	setRawData(name, new_settings["Name"]);
}

void Device::initialize()
{
	PRINTLN("\t- Name: " << name << "\n\t- UID: " << UID << "\n\t- Type: " << std::to_string(my_type))
}

void Device::prepForDestruction()
{
	PRINTLN("\t- Name: " << name << "\n\t- UID: " << UID << "\n\t- Type: " << std::to_string(my_type))
}

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
Environment::Environment(bool enable_ambient_lighting, glm::vec3 init_ambient_color, float init_ambient_strength)
: ambient_lighting_enabled(enable_ambient_lighting), ambient_light_color(init_ambient_color), ambient_light_strength(init_ambient_strength)
{
	my_type = graphx::classes::ENVIRONMENT;
	name = "Untitled Environment";
}

void Environment::loadSettings(graphx::gSettings new_settings)
{
	Device::loadSettings(new_settings);

	setRawData(ambient_lighting_enabled, new_settings["AmbientLightingEnabled"]);
	setRawData(ambient_light_color, new_settings["AmbientLightingColor"]);
	setRawData(ambient_light_strength, new_settings["AmbientLightingStrength"]);
}


glm::vec3 Environment::getAmbientLight()
{
	return ambient_light_color * ambient_light_strength * (int)ambient_lighting_enabled;
}
//
// Material
//
Material::Material()
{
	my_type = graphx::classes::MATERIAL;
	name = "Untitled Material";
}

Material::Material(bool is_fullbright, glm::vec3 init_color)
: embedded_texture_specular(NO_TEXTURE_jpg), color(init_color), specular_strength(0.0f), mat_fullbright(is_fullbright)
{}

Material::Material(unsigned char *init_diffuse_texture, unsigned char *init_specular_texture, int init_specular_sharpness, float init_specular_strength, glm::vec3 init_color)
: embedded_texture_diffuse(init_diffuse_texture), embedded_texture_specular(init_specular_texture), color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
{}

Material::Material(glm::vec3 init_color, float init_specular_strength, unsigned int init_specular_sharpness)
: embedded_texture_specular(FLAT_SPEC_jpg), color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
{}

void Material::loadSettings(graphx::gSettings new_settings)
{
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
	unsigned char *t_data = stbi_load_from_memory(texture_buffer, 1600*1600, &t_width, &t_height, &t_channels, STBI_rgb);

	if(!t_data)
	{
		std::cerr << "Failed to load texture!" << std::endl;
		return 20;
	}

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, t_data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, t_data);
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
	my_type = graphx::classes::MESH;
	name = "Untitled Mesh";
}

Mesh::~Mesh()
{
	material->prepForDestruction();
	material = nullptr;
	delete material;
}

void Mesh::loadSettings(graphx::gSettings new_settings)
{
	Device::loadSettings(new_settings);

	gMeshData mesh_data = gMeshData(ERROR_VERTS, ERROR_INDICES, VAO_HANDMADE);

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
	my_type = graphx::classes::SPRITE;
	name = "Untitled Sprite";
}

void Sprite::loadSettings(graphx::gSettings new_settings)
{
	Mesh::loadSettings(new_settings);
}