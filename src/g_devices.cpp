#include "g_devices.hpp"
// #include "sanity.hpp"
#include "graphx_classes.hpp"
#include "t_settings.hpp"
#include <gmath.hpp>
#include <glm/gtx/component_wise.hpp>
//---------
// Collider
//---------
Collider::~Collider()
{
	if(jolt_physics_system.GetBodyInterface().IsAdded(body_id)/* || !body_id.IsInvalid()*/)
		J_RemoveAndDestroyBody(body_id);
}

void Collider::loadSettings()
{
	Device::loadSettings();

	getSetting(overrides_actor_transform, settings["Rigidbody"]);
	getSetting(overrides_actor_transform, settings["ControlActor"]);
	getSetting(overrides_actor_transform, settings["OverrideActor"]);
	getSetting(motion_type, settings["MotionType"]);
	getSetting(object_layer, settings["ObjectLayer"]);
	getSetting(activation, settings["Activation"]);
	getSetting(shape, settings["Shape"]);
	getSetting(forever_alone, settings["ForeverAlone"]);
	getSetting(position, settings["Position"]);
	getSetting(euler_angles, settings["Rotation"]);
	getSetting(local_position, settings["LocalPosition"]);
	getSetting(local_euler_angles, settings["LocalRotation"]);
	getSetting(scale, settings["Scale"]);

	shape_arguments = std::make_tuple(scale, glm::max(glm::max(scale[0], scale[1]), scale[2]), scale[1]);
	JPH::RVec3 body_position = gmath::convertMath<JPH::Vec3>(position) + gmath::convertMath<JPH::Vec3>(local_position);
	JPH::Quat body_quaternion = JPH::Quat::sEulerAngles(gmath::convertMath<JPH::Vec3>(glm::radians(euler_angles))) * JPH::Quat::sEulerAngles(gmath::convertMath<JPH::Vec3>(glm::radians(local_euler_angles)));

	body_settings = JPH::BodyCreationSettings(J_CreateAShape(shape, shape_arguments), body_position, body_quaternion, motion_type, object_layer);
	body_id = jolt_physics_system.GetBodyInterface().CreateAndAddBody(body_settings, activation);
	jolt_physics_system.GetBodyInterface().SetFriction(body_id, friction);
}

JPH::BodyCreationSettings* Collider::getBodySettings()
{ return &body_settings; }

const JPH::BodyID& Collider::getBodyID()
{ return body_id; }

//
// Environment
//
void Environment::loadSettings()
{
	Device::loadSettings();

	bool ambient_light_enabled = (ambient_light_amount > 0.0f);

	getSetting(ambient_light_amount, settings["AmbientLightAmount"]);
	getSetting(ambient_light_color, settings["AmbientLightColor"]);
	getSetting(ambient_light_enabled, settings["AmbientLightEnabled"]);

	ambient_light_amount *= ambient_light_enabled;
}

//
// Texture
//
Texture::Texture()
{
	my_type = &graphx::classes::TEXTURE;
	uid.name = "Untitled Texture";
}

Texture::Texture(std::vector<unsigned char *> init_texture_data, std::vector<unsigned int> init_texture_size)
{
	my_type = &graphx::classes::TEXTURE;
	uid.name = "Untitled Texture";
	texture_data = init_texture_data;
	texture_size = init_texture_size;
}

Texture::Texture(std::vector<const char *> init_texture_data, std::vector<unsigned int> init_texture_size)
{
	my_type = &graphx::classes::TEXTURE;
	uid.name = "Untitled Texture";
	texture_size = init_texture_size;
	texture_data.clear();
	for(const char *some_texture_data : init_texture_data)
		texture_data.insert(texture_data.end(), reinterpret_cast<unsigned char *>(const_cast<char *>(some_texture_data)));
}

Texture::Texture(std::vector<std::string > init_texture_data, std::vector<unsigned int> init_texture_size)
{
	my_type = &graphx::classes::TEXTURE;
	uid.name = "Untitled Texture";
	texture_size = init_texture_size;
	texture_data.clear();
	for(std::string some_texture_data : init_texture_data)
		texture_data.insert(texture_data.end(), reinterpret_cast<unsigned char *>(const_cast<char *>(some_texture_data.c_str())));
}

Texture::Texture(unsigned char *init_texture_data, unsigned int init_texture_size)
{
	my_type = &graphx::classes::TEXTURE;
	uid.name = "Untitled Texture";
	texture_data = {init_texture_data};
	texture_size = {init_texture_size};
}

Texture::Texture(const char *init_texture_data, unsigned int init_texture_size)
{
	my_type = &graphx::classes::TEXTURE;
	uid.name = "Untitled Texture";
	texture_size = {init_texture_size};
	texture_data = {reinterpret_cast<unsigned char *>(const_cast<char *>(init_texture_data))};
}

Texture::Texture(std::string init_texture_data, unsigned int init_texture_size)
{
	my_type = &graphx::classes::TEXTURE;
	uid.name = "Untitled Texture";
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
	my_type = &graphx::classes::MATERIAL;
	uid.name = "Untitled Material";
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

void Material::loadSettings()
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
// Model
//
Model::Model()
{
	my_type = &graphx::classes::MESH;
	uid.name = "Untitled Model";
}

Model::Model(Material *new_material, std::string init_mesh_data_name)
{
	my_type = &graphx::classes::MESH;
	uid.name = "Untitled Model";
	material = new_material;
}

Model::Model(std::string init_mesh_data_name)
{
	my_type = &graphx::classes::MESH;
	uid.name = "Untitled Model";
	mesh_data_name = init_mesh_data_name;
}

void Model::prepForDestruction()
{
	Device::prepForDestruction();

	if(material != nullptr)
		material->prepForDestruction();

	material = nullptr;
	delete material;
}

void Model::loadSettings()
{
	Device::loadSettings(new_settings);

	getSetting(material, settings["Material"]);
	getSetting(mesh_data_name, settings["Mesh"]);
}

//
// Sprite
//
Sprite::Sprite(std::string init_name)
: Model(init_name)
{
	my_type = &graphx::classes::SPRITE;
	mesh_data_name = GRAPHX_QUAD;
	uid.name = init_name;
}

void Sprite::loadSettings()
{
	Model::loadSettings(new_settings);
	mesh_data_name = GRAPHX_QUAD; // Override
}