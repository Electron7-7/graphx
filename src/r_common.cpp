#include "r_common.hpp"
#include "sanity.hpp"
#include "t_settings.hpp"
#include "g_jolt.hpp"
#include <gmath.hpp>
#include <glm/gtx/component_wise.hpp>

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

template<> void GLShader::setUniform<glm::bvec3>(const std::string &name, glm::bvec3 value) const
{
	glm::vec3 bool_as_float((float)value.x, (float)value.y, (float)value.z);
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(bool_as_float));
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
// Device
//
Device::Device()
{
	my_type = graphx::classes::DEVICE;
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

	getSetting(name, new_settings["Name"]);
}

void Device::initialize()
{}

void Device::prepForDestruction()
{
	if(ready_to_destroy)
		return;
	ready_to_destroy = true;
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

	getSetting(ambient_lighting_enabled, settings["AmbientLightingEnabled"]);
	getSetting(ambient_light_color, settings["AmbientLightingColor"]);
	getSetting(ambient_light_strength, settings["AmbientLightingStrength"]);
}


glm::vec3 Environment::getAmbientLight()
{
	return ambient_light_color * ambient_light_strength * (int)ambient_lighting_enabled;
}

//
// Texture
//
Texture::Texture(bool init_is_cubemap)
{
	my_type = graphx::classes::TEXTURE;
	name = "Untitled Texture";
	is_cubemap = init_is_cubemap;
}

Texture::Texture(unsigned char *init_texture_data, bool init_is_cubemap)
{
	my_type = graphx::classes::TEXTURE;
	name = "Untitled Texture";
	texture_data = {init_texture_data};
	is_cubemap = init_is_cubemap;
}

Texture::Texture(const char *init_texture_data, bool init_is_cubemap)
{
	my_type = graphx::classes::TEXTURE;
	name = "Untitled Texture";
	texture_data = {reinterpret_cast<unsigned char *>(const_cast<char *>(init_texture_data))};
	is_cubemap = init_is_cubemap;
}

Texture::Texture(std::string init_texture_data, bool init_is_cubemap)
{
	my_type = graphx::classes::TEXTURE;
	name = "Untitled Texture";
	texture_data = {reinterpret_cast<unsigned char *>(const_cast<char *>(init_texture_data.c_str()))};
	is_cubemap = init_is_cubemap;
}

void Texture::loadSettings(graphx::gSettings new_settings)
{
	Device::loadSettings(new_settings);

	getSetting(is_cubemap, settings["IsCubemap"]);
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
: color(init_color), specular_strength(0.0f), mat_fullbright(is_fullbright)
{}

Material::Material(std::string init_diffuse_texture_name, std::string init_specular_texture_name, int init_specular_sharpness, float init_specular_strength, glm::vec3 init_color)
: diffuse_texture_name(init_diffuse_texture_name), specular_texture_name(init_specular_texture_name), color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
{}

Material::Material(glm::vec3 init_color, float init_specular_strength, unsigned int init_specular_sharpness)
: color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
{}

void Material::loadSettings(graphx::gSettings new_settings)
{
	Device::loadSettings(new_settings);

	getSetting(diffuse_texture_name, settings["DiffuseTexture"]);
	getSetting(specular_texture_name, settings["SpecularTexture"]);
	getSetting(color, settings["Color"]);
	getSetting(specular_sharpness, settings["SpecularSharpness"]);
	getSetting(specular_strength, settings["SpecularStrength"]);
	getSetting(mat_fullbright, settings["mat_fullbright"]);

	if(mat_fullbright && diffuse_texture_name == MISSING_TEXTURE)
		diffuse_texture_name = NO_TEXTURE;

	if(specular_texture_name == NO_TEXTURE)
		specular_strength = 0.0f;
}


//
// MeshData
//
MeshData::MeshData()
{}

MeshData::MeshData(int init_vao_index, std::vector<glm::vec3> init_positions, std::vector<glm::vec3> init_normals, std::vector<glm::vec2> init_uvs, std::vector<glm::vec3> init_colors, std::vector<gmath::uintvec3> init_indices)
{
	// These for loops make sure that every vertex has normal, uv, and color data
	for(int i = init_normals.size() ; i < init_positions.size() ; i++)
		init_normals.insert(init_normals.end(), glm::vec3(0.0f, 0.0f, 0.0f));

	for(int i = init_uvs.size() ; i < init_positions.size() ; i++)
		init_uvs.insert(init_uvs.end(), glm::vec2(0.0f, 0.0f));

	for(int i = init_colors.size() ; i < init_positions.size() ; i++)
		init_colors.insert(init_colors.end(), glm::vec3(1.0f, 1.0f, 1.0f));

	VAO_index = init_vao_index;
	vertex_positions = init_positions;
	vertex_normals = init_normals;
	vertex_uvs = init_uvs;
	vertex_colors = init_colors;

	if(init_indices.empty())
		for(int i = 0 ; i < vertex_positions.size() * 3; i += 3)
			vertex_indices.insert(vertex_indices.end(), gmath::uintvec3(i, i+1, i+2));
	else
		vertex_indices = init_indices;
}

MeshData::MeshData(int init_vao_index, std::vector<float> init_positions, std::vector<float> init_normals, std::vector<float> init_uvs, std::vector<float> init_colors, std::vector<unsigned int> init_indices)
{
	// These for loops make sure that every vertex has normal, uv, and color data
	for(int i = init_normals.size() ; i < init_positions.size() ; i++)
		init_normals.insert(init_normals.end(), 0.0f);

	for(int i = init_uvs.size() ; i < (2 * init_positions.size() / 3) ; i++)
		init_uvs.insert(init_uvs.end(), 0.0f);

	for(int i = init_colors.size() ; i < init_positions.size() ; i++)
		init_colors.insert(init_colors.end(), 1.0f);

	if(!init_indices.empty())
		for(int i = init_indices.size() ; i < init_positions.size() ; i++)
			init_colors.insert(init_colors.end(), 1.0f);

	VAO_index = init_vao_index;
	for(int it = 0,uv_it = 0 ; it < init_positions.size() ; it += 3,uv_it += 2)
	{
		vertex_positions.insert(vertex_positions.end(), glm::vec3(init_positions[it], init_positions[it + 1], init_positions[it + 2]));
		vertex_normals.insert(vertex_normals.end(), glm::vec3(init_normals[it], init_normals[it + 1], init_normals[it + 2]));
		vertex_uvs.insert(vertex_uvs.end(), glm::vec2(init_uvs[uv_it], init_uvs[uv_it + 1]));
		vertex_colors.insert(vertex_colors.end(), glm::vec3(init_colors[it], init_colors[it + 1], init_colors[it + 2]));
		if(!init_indices.empty())
			vertex_indices.insert(vertex_indices.end(), gmath::uintvec3(init_indices[it], init_indices[it + 1], init_indices[it + 2]));
	}

	if(init_indices.empty())
		for(int i = 0 ; i < vertex_positions.size() * 3; i += 3)
			vertex_indices.insert(vertex_indices.end(), gmath::uintvec3(i, i+1, i+2));
}

const std::vector<float> MeshData::vertices()
{
	std::vector<float> vertices;
	for(int i = 0 ; i < vertex_positions.size() ; i++)
	{
		vertices.insert(vertices.end(),
		{
			vertex_positions.at(i).x,
			vertex_positions.at(i).y,
			vertex_positions.at(i).z,
			vertex_normals.at(i).x,
			vertex_normals.at(i).y,
			vertex_normals.at(i).z,
			vertex_uvs.at(i).x,
			vertex_uvs.at(i).y,
			vertex_colors.at(i).x,
			vertex_colors.at(i).y,
			vertex_colors.at(i).z
		});
	}
	return vertices;
}

const std::vector<unsigned int> MeshData::indices()
{
	std::vector<unsigned int> indices;
	for(int i = 0 ; i < vertex_indices.size() ; i++)
	{
		indices.insert(indices.end(),
		{
			vertex_indices.at(i).x(),
			vertex_indices.at(i).y(),
			vertex_indices.at(i).z()
		});
	}
	return indices;
}

size_t MeshData::vertices_count()
{
	return (vertex_positions.size());
}

size_t MeshData::vertices_size()
{
	return
	(
		(3 * sizeof(float) * vertex_positions.size()) +
		(3 * sizeof(float) * vertex_normals.size())   +
		(2 * sizeof(float) * vertex_uvs.size())       +
		(3 * sizeof(float) * vertex_colors.size())
	);
}

size_t MeshData::indices_count()
{
	return (vertex_indices.size() * 3);
}

size_t MeshData::indices_size()
{
	return (3 * sizeof(unsigned int) * vertex_indices.size());
}

void MeshData::addVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, glm::vec3 color)
{
	vertex_positions.insert(vertex_positions.end(), position);
	vertex_normals.insert(vertex_normals.end(), normal);
	vertex_uvs.insert(vertex_uvs.end(), uv);
	vertex_colors.insert(vertex_colors.end(), color);
}

void MeshData::addVertex(float position_x, float position_y, float position_z, float normal_x, float normal_y, float normal_z, float uv_x, float uv_y, float color_x, float color_y, float color_z)
{
	vertex_positions.insert(vertex_positions.end(), glm::vec3(position_x, position_y, position_z));
	vertex_normals.insert(vertex_normals.end(), glm::vec3(normal_x, normal_y, normal_z));
	vertex_uvs.insert(vertex_uvs.end(), glm::vec2(uv_x, uv_y));
	vertex_colors.insert(vertex_colors.end(), glm::vec3(color_x, color_y, color_z));
}

void MeshData::addVertex(std::vector<float> vertex)
{
	vertex_positions.insert(vertex_positions.end(), glm::vec3(vertex[0], vertex[1], vertex[2]));
	vertex_normals.insert(vertex_normals.end(), glm::vec3(vertex[3], vertex[4], vertex[5]));
	vertex_uvs.insert(vertex_uvs.end(), glm::vec2(vertex[6], vertex[7]));
	vertex_colors.insert(vertex_colors.end(), glm::vec3(vertex[8], vertex[9], vertex[10]));
}

void MeshData::addIndex(gmath::uintvec3 indices)
{
	vertex_indices.insert(vertex_indices.end(), indices);
}

void MeshData::addIndex(unsigned int index_1, unsigned int index_2, unsigned int index_3)
{
	vertex_indices.insert(vertex_indices.end(), gmath::uintvec3(index_1, index_2, index_3));
}

void MeshData::fixOBJData()
{
	float max_coordinate = 0.0f;
	float min_coordinate = 0.0f;

	for(glm::vec3 vertex : vertex_positions)
	{
		if(std::abs(vertex.x) > 1 || std::abs(vertex.y) > 1 || std::abs(vertex.z) > 1)
		{
			float biggest_pos = glm::compMax(vertex);
			float smallest_pos = glm::compMin(vertex);
			if(biggest_pos > max_coordinate)
				max_coordinate = biggest_pos;
			if(smallest_pos < min_coordinate)
				min_coordinate = smallest_pos;
		}
	}

	for(glm::vec3 vertex : vertex_positions)
	{
		for(float component : vertex)
		{
			component = (component - min_coordinate) / (max_coordinate - min_coordinate);
		}
	}

	vertex_indices.clear();
	for(int i = 0 ; i < vertex_positions.size() * 3; i += 3)
	{
		vertex_indices.insert(vertex_indices.end(), gmath::uintvec3(i, i + 1, i + 2));
	}
}

//
// Mesh
//
Mesh::Mesh()
{
	my_type = graphx::classes::MESH;
	name = "Untitled Mesh";
}

Mesh::Mesh(Material *new_material)
{
	my_type = graphx::classes::MESH;
	name = "Untitled Mesh";
	material = new_material;
}

void Mesh::prepForDestruction()
{
	Device::prepForDestruction();

	if(material != nullptr)
		material->prepForDestruction();

	material = nullptr;
	delete material;
}

void Mesh::loadSettings(graphx::gSettings new_settings)
{
	Device::loadSettings(new_settings);

	getSetting(material, settings["Material"]);
	getSetting(mesh_data_name, settings["MeshData"]);
}

//
// Sprite
//
Sprite::Sprite()
: Mesh()
{
	my_type = graphx::classes::SPRITE;
	name = "Untitled Sprite";
	mesh_data_name = GRAPHX_QUAD;
}

void Sprite::loadSettings(graphx::gSettings new_settings)
{
	Mesh::loadSettings(new_settings);
}

//
// RenderCmd
//
RenderCmd::RenderCmd(LightRenderCmd &light_render_command, glm::vec3 light_debug_material_color)
{
	is_light_debug_mesh = true;
	current_render_state = light_render_command.current_render_state;
	previous_render_state = light_render_command.previous_render_state;
	mesh_material = Material(LIGHT_DEBUGGING, NO_TEXTURE, 8, 0.0f, light_debug_material_color);
	mesh_data_name = GRAPHX_CUBE;
}

bool RenderCmd::isRenderable()
{
	return ((current_render_state != nullptr || previous_render_state != nullptr) && !mesh_data_name.empty());
}

//
// LightRenderCmd
//
bool LightRenderCmd::renderDebugMesh()
{
	return ((current_render_state != nullptr || previous_render_state != nullptr));
}

//
// PrimitiveRenderCmd
//
PrimitiveRenderCmd::PrimitiveRenderCmd(glm::vec3 new_vertex_1, glm::vec3 new_vertex_2, glm::vec3 new_vertex_color)
{
	primitive_type = graphx::identifiers::primitive::LINE;
	vertex_1 = new_vertex_1;
	vertex_2 = new_vertex_2;
	colors_1 = new_vertex_color;
	colors_2 = new_vertex_color;
}
PrimitiveRenderCmd::PrimitiveRenderCmd(glm::vec3 new_vertex_1, glm::vec3 new_vertex_2, glm::vec3 new_vertex_3, glm::vec3 new_vertex_color)
{
	primitive_type = graphx::identifiers::primitive::TRIANGLE;
	vertex_1 = new_vertex_1;
	vertex_2 = new_vertex_2;
	vertex_3 = new_vertex_3;
	colors_1 = new_vertex_color;
	colors_2 = new_vertex_color;
	colors_3 = new_vertex_color;
}
PrimitiveRenderCmd::PrimitiveRenderCmd(JPH::RVec3Arg new_vertex_1, JPH::RVec3Arg new_vertex_2, JPH::ColorArg new_vertex_color)
{
	primitive_type = graphx::identifiers::primitive::LINE;
	vertex_1 = gmath::convertMath<glm::vec3>(new_vertex_1);
	vertex_2 = gmath::convertMath<glm::vec3>(new_vertex_2);
	colors_1 = gmath::convertMath<glm::vec3>(new_vertex_color);
	colors_2 = gmath::convertMath<glm::vec3>(new_vertex_color);
}
PrimitiveRenderCmd::PrimitiveRenderCmd(JPH::RVec3Arg new_vertex_1, JPH::RVec3Arg new_vertex_2, JPH::RVec3Arg new_vertex_3, JPH::ColorArg new_vertex_color)
{
	primitive_type = graphx::identifiers::primitive::TRIANGLE;
	vertex_1 = gmath::convertMath<glm::vec3>(new_vertex_1);
	vertex_2 = gmath::convertMath<glm::vec3>(new_vertex_2);
	vertex_3 = gmath::convertMath<glm::vec3>(new_vertex_3);
	colors_1 = gmath::convertMath<glm::vec3>(new_vertex_color);
	colors_2 = gmath::convertMath<glm::vec3>(new_vertex_color);
	colors_3 = gmath::convertMath<glm::vec3>(new_vertex_color);
}