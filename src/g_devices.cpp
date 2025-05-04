#include "g_devices.hpp"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "t_settings.hpp"
#include <gmath.hpp>
#define GLM_ENABLE_EXPERIMENTAL
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
	configureBaseVariables(this);

	settings.getVariable("Rigidbody", overrides_actor_transform);
	settings.getVariable("ControlActor", overrides_actor_transform);
	settings.getVariable("OverrideActor", overrides_actor_transform);
	settings.getVariable("MotionType", motion_type);
	settings.getVariable("ObjectLayer", object_layer);
	settings.getVariable("Activation", activation);
	settings.getVariable("Shape", shape);
	settings.getRawData("ForeverAlone", forever_alone);
	settings.getRawData("Position", position);
	settings.getRawData("Rotation", euler_angles);
	settings.getRawData("LocalPosition", local_position);
	settings.getRawData("LocalRotation", local_euler_angles);
	settings.getRawData("Scale", scale);

	shape_arguments = std::make_tuple(scale, glm::max(glm::max(scale[0], scale[1]), scale[2]), scale[1]);
	reset_position = gmath::convertMath<JPH::Vec3>(position) + gmath::convertMath<JPH::Vec3>(local_position);
	reset_quaternion = JPH::Quat::sEulerAngles(gmath::convertMath<JPH::Vec3>(glm::radians(euler_angles))) * JPH::Quat::sEulerAngles(gmath::convertMath<JPH::Vec3>(glm::radians(local_euler_angles)));

	switch(shape)
	{
	case graphx::jolt::shapes::BOX:
		body_shape = std::shared_ptr<JPH::BoxShape>(new JPH::BoxShape(gmath::convertMath<JPH::Vec3>(std::get<0>(shape_arguments))));
	case graphx::jolt::shapes::SPHERE:
		body_shape = std::shared_ptr<JPH::SphereShape>(new JPH::SphereShape(std::get<1>(shape_arguments)));
	case graphx::jolt::shapes::CAPSULE:
		body_shape = std::shared_ptr<JPH::CapsuleShape>(new JPH::CapsuleShape(std::get<2>(shape_arguments), std::get<1>(shape_arguments)));
	case graphx::jolt::shapes::CYLINDER:
		body_shape = std::shared_ptr<JPH::CylinderShape>(new JPH::CylinderShape(std::get<2>(shape_arguments), std::get<1>(shape_arguments)));
	};

	body_settings = JPH::BodyCreationSettings(body_shape.get(), reset_position, reset_quaternion, motion_type, object_layer);
	body_id = jolt_physics_system.GetBodyInterface().CreateAndAddBody(body_settings, activation);
	jolt_physics_system.GetBodyInterface().SetFriction(body_id, friction);
}

void Collider::reset_to_default_transformation_for_testing()
{ jolt_physics_system.GetBodyInterface().SetPositionAndRotation(body_id, reset_position, reset_quaternion, JPH::EActivation::Activate); }

JPH::BodyCreationSettings* Collider::getBodySettings()
{ return &body_settings; }

const JPH::BodyID& Collider::getBodyID()
{ return body_id; }

//
// Environment
//
void Environment::loadSettings()
{
	configureBaseVariables(this);

	bool ambient_light_enabled = (ambient_light_amount > 0.0f);

	settings.getRawData("AmbientLightAmount", ambient_light_amount);
	settings.getRawData("AmbientLightColor", ambient_light_color);
	settings.getRawData("AmbientLightEnabled", ambient_light_enabled);

	ambient_light_amount *= ambient_light_enabled;
}

//
// Texture
//
Texture::Texture(std::vector<unsigned char*> init_texture_data, std::vector<unsigned int> init_texture_size)
: Device("Untitled Texture"), texture_data(init_texture_data), texture_size(init_texture_size)
{}

Texture::Texture(unsigned char* init_texture_data, unsigned int init_texture_size)
: Texture(std::vector<unsigned char*>{init_texture_data}, std::vector<unsigned int>{init_texture_size})
{}

Texture::Texture(const std::string& init_name)
: Device(init_name)
{}

void Texture::loadSettings()
{ configureBaseVariables(this); }

//
// Material
//
Material::Material(bool is_fullbright, glm::vec3 init_color)
: Device("Untitled Material"), color(init_color), specular_strength(0.0f), mat_fullbright(is_fullbright)
{}

Material::Material(std::string init_diffuse_texture_name, std::string init_specular_texture_name, int init_specular_sharpness, float init_specular_strength, glm::vec3 init_color)
: Device("Untitled Material"), diffuse_texture_name(init_diffuse_texture_name), specular_texture_name(init_specular_texture_name), color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
{}

Material::Material(glm::vec3 init_color, float init_specular_strength, unsigned int init_specular_sharpness)
: Device("Untitled Material"), color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
{}

void Material::loadSettings()
{
	configureBaseVariables(this);

	settings.getVariable("DiffuseTexture", diffuse_texture_name);
	settings.getVariable("SpecularTexture", specular_texture_name);
	settings.getRawData("Color", color);
	settings.getRawData("Alpha", color_alpha);
	settings.getRawData("SpecularSharpness", specular_sharpness);
	settings.getRawData("SpecularStrength", specular_strength);
	settings.getRawData("mat_fullbright", mat_fullbright);

	if(mat_fullbright && diffuse_texture_name == MISSING_TEXTURE)
		diffuse_texture_name = NO_TEXTURE;

	if(specular_texture_name == NO_TEXTURE)
		specular_strength = 0.0f;
}

//
// Model
//
Model::Model(const Material& new_material, const std::string& new_mesh_data_name)
: Device("Untitled Model"), material_base(new_material), material(&material_base), mesh_data_name(new_mesh_data_name)
{}

Model::Model(std::shared_ptr<Material> new_material, const std::string& new_mesh_data_name)
: Device("Untitled Model"), material(new_material), mesh_data_name(new_mesh_data_name)
{}

void Model::loadSettings()
{
	configureBaseVariables(this);

	settings.getVariable("Material", material);
	settings.getVariable("Mesh", mesh_data_name);
	settings.getVariable("MeshData", mesh_data_name);
}

//
// Sprite
//
void Sprite::loadSettings()
{
	// Model::loadSettings();
	configureBaseVariables(this);

	// Todo: expand configureBaseVariables to include other types (like Model)
	settings.getVariable("Material", material);
	settings.getVariable("Mesh", mesh_data_name);
	settings.getVariable("MeshData", mesh_data_name);

	mesh_data_name = GRAPHX_QUAD; // Override
}