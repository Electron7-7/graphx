#include "r_common.hpp"
#include "sanity.hpp"
#include "graphx_classes_namespace.hpp"
#include <gmath.hpp>
#include <glm/gtx/component_wise.hpp>

//
// GLShader
//
GLShader::GLShader(std::string vertex_shader_code, std::string fragment_shader_code)
{
	const char *v_shader_code = vertex_shader_code.c_str();
	const char *f_shader_code = fragment_shader_code.c_str();

	unsigned int vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v_shader_code, nullptr);
	glCompileShader(vertex);
	GLShaderErrorHandler(vertex);

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f_shader_code, nullptr);
	glCompileShader(fragment);
	GLShaderErrorHandler(fragment);

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);
	GLShaderErrorHandler(id, true);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void GLShader::GLShaderErrorHandler(const unsigned int& shader_id, const bool is_program_linking)
{
	// https://stackoverflow.com/a/63420289
	int v_result = GL_FALSE;
	int info_log_length;
	std::string shader_error_type = "Shader Compilation";
	if(is_program_linking)
	{
		shader_error_type = "Program Linking";
		glGetProgramiv(shader_id, GL_LINK_STATUS, &v_result);
		glGetProgramiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	}
	else
	{
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &v_result);
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	}
	if(info_log_length > 0)
	{
		std::vector<char> shader_error_message(info_log_length + 1);
		if(is_program_linking)
			glGetProgramInfoLog(shader_id, info_log_length, nullptr, shader_error_message.data());
		else
			glGetShaderInfoLog(shader_id, info_log_length, nullptr, shader_error_message.data());

		PRINTERR("GLSL " << shader_error_type << " Error(s):\n" << shader_error_message.data())
	}
}

template<> void GLShader::setUniform<bool>(const std::string &name, bool value) const
{
	glProgramUniform1i(id, glGetUniformLocation(id, name.c_str()), static_cast<int>(value));
}

template<> void GLShader::setUniform<int>(const std::string &name, int value) const
{
	glProgramUniform1i(id, glGetUniformLocation(id, name.c_str()), value);
}

template<> void GLShader::setUniform<float>(const std::string &name, float value) const
{
	glProgramUniform1f(id, glGetUniformLocation(id, name.c_str()), value);
}

template<> void GLShader::setUniform<glm::vec2>(const std::string &name, glm::vec2 value) const
{
	glProgramUniform2fv(id, glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

template<> void GLShader::setUniform<glm::vec3>(const std::string &name, glm::vec3 value) const
{
	glProgramUniform3fv(id, glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

template<> void GLShader::setUniform<glm::vec4>(const std::string &name, glm::vec4 value) const
{
	glProgramUniform4fv(id, glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

template<> void GLShader::setUniform<glm::mat3>(const std::string &name, glm::mat3 value) const
{
	glProgramUniformMatrix3fv(id, glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

template<> void GLShader::setUniform<glm::mat4>(const std::string &name, glm::mat4 value) const
{
	glProgramUniformMatrix4fv(id, glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

//
// Device
//
Device::Device()
{
	my_type = graphx::classes::DEVICE;
}

graphx::gClass &Device::getType()
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
Environment::Environment(std::string init_name, bool enable_ambient_light, float init_ambient_light_amount, glm::vec3 init_ambient_light_color)
: ambient_light_color(init_ambient_light_color), ambient_light_amount(init_ambient_light_amount * enable_ambient_light)
{
	name = init_name;
	my_type = graphx::classes::ENVIRONMENT;
}

void Environment::loadSettings(graphx::gSettings new_settings)
{
	Device::loadSettings(new_settings);

	getSetting(ambient_light_amount, settings["AmbientLightAmount"]);
	getSetting(ambient_light_color, settings["AmbientLightColor"]);
}

//
// Texture
//
Texture::Texture()
{
	my_type = graphx::classes::TEXTURE;
	name = "Untitled Texture";
}

Texture::Texture(std::vector<unsigned char *> init_texture_data, std::vector<unsigned int> init_texture_size)
{
	my_type = graphx::classes::TEXTURE;
	name = "Untitled Texture";
	texture_data = init_texture_data;
	texture_size = init_texture_size;
}

Texture::Texture(std::vector<const char *> init_texture_data, std::vector<unsigned int> init_texture_size)
{
	my_type = graphx::classes::TEXTURE;
	name = "Untitled Texture";
	texture_size = init_texture_size;
	texture_data.clear();
	for(const char *some_texture_data : init_texture_data)
		texture_data.insert(texture_data.end(), reinterpret_cast<unsigned char *>(const_cast<char *>(some_texture_data)));
}

Texture::Texture(std::vector<std::string > init_texture_data, std::vector<unsigned int> init_texture_size)
{
	my_type = graphx::classes::TEXTURE;
	name = "Untitled Texture";
	texture_size = init_texture_size;
	texture_data.clear();
	for(std::string some_texture_data : init_texture_data)
		texture_data.insert(texture_data.end(), reinterpret_cast<unsigned char *>(const_cast<char *>(some_texture_data.c_str())));
}

Texture::Texture(unsigned char *init_texture_data, unsigned int init_texture_size)
{
	my_type = graphx::classes::TEXTURE;
	name = "Untitled Texture";
	texture_data = {init_texture_data};
	texture_size = {init_texture_size};
}

Texture::Texture(const char *init_texture_data, unsigned int init_texture_size)
{
	my_type = graphx::classes::TEXTURE;
	name = "Untitled Texture";
	texture_size = {init_texture_size};
	texture_data = {reinterpret_cast<unsigned char *>(const_cast<char *>(init_texture_data))};
}

Texture::Texture(std::string init_texture_data, unsigned int init_texture_size)
{
	my_type = graphx::classes::TEXTURE;
	name = "Untitled Texture";
	texture_size = {init_texture_size};
	texture_data = {reinterpret_cast<unsigned char *>(const_cast<char *>(init_texture_data.c_str()))};
}

void Texture::loadSettings(graphx::gSettings new_settings)
{
	Device::loadSettings(new_settings);
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
	getSetting(color_alpha, settings["Alpha"]);
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
	for(int i = 0 ; i < vertex_positions.size(); i += 3)
	{
		vertex_indices.insert(vertex_indices.end(), gmath::uintvec3(i, i + 1, i + 2));
	}
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

//
// Mesh
//
Mesh::Mesh()
{
	my_type = graphx::classes::MESH;
	name = "Untitled Mesh";
}

Mesh::Mesh(Material *new_material, std::string init_mesh_data_name)
{
	my_type = graphx::classes::MESH;
	name = "Untitled Mesh";
	material = new_material;
}

Mesh::Mesh(std::string init_mesh_data_name)
{
	my_type = graphx::classes::MESH;
	name = "Untitled Mesh";
	mesh_data_name = init_mesh_data_name;
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
Sprite::Sprite(std::string init_name)
: Mesh(init_name)
{
	my_type = graphx::classes::SPRITE;
	mesh_data_name = GRAPHX_QUAD;
	name = init_name;
}

void Sprite::loadSettings(graphx::gSettings new_settings)
{
	Mesh::loadSettings(new_settings);
	mesh_data_name = GRAPHX_QUAD; // Override
}

//
// Character
//
Character::Character(unsigned int init_texture_id, int init_size_x, int init_size_y, int init_bearing_x, int init_bearing_y, int init_advance)
{
	texture_id = init_texture_id;
	size_x = init_size_x;
	size_y = init_size_y;
	bearing_x = init_bearing_x;
	bearing_y = init_bearing_y;
	advance = init_advance;
}

Character::Character(unsigned int init_texture_id, glm::vec2 init_size, glm::vec2 init_bearing, int init_advance)
: Character(init_texture_id, init_size.x, init_size.y, init_bearing.x, init_bearing.y, init_advance)
{}

//
// Font
//
Font::Font(std::string init_font_name)
: font_name(init_font_name)
{}

//
// LightRenderCmd
//
bool LightRenderCmd::isValid() const
{
	return (light_type != graphx::gClass::INVALID_TYPE);
}

//
// RenderCmd
//
bool RenderCmd::isValid() const
{
	return ((current_render_state != nullptr || previous_render_state != nullptr) && !mesh_data_name.empty());
}

//
// TextRenderCmd
//
TextRenderCmd::TextRenderCmd(std::string init_text, float init_position_x, float init_position_y, float init_scale, glm::vec3 init_color)
{
	font_name = "Arial";
	text = init_text;
	position_x = init_position_x;
	position_y = init_position_y;
	scale = init_scale;
	color = init_color;
}

TextRenderCmd::TextRenderCmd(std::string init_font_name, std::string init_text, float init_position_x, float init_position_y, float init_scale, glm::vec3 init_color)
: TextRenderCmd(init_text, init_position_x, init_position_y, init_scale, init_color)
{
	font_name = (font_map.contains(init_font_name)) ? init_font_name : "Arial";
}

bool TextRenderCmd::isValid() const
{
	return (font_map.contains(font_name) && scale > 0.0f);
}

bool TextRenderCmd::is3D() const
{
	return (render_state != nullptr);
}
