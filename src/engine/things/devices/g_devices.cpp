#include "g_devices.hpp"
#include "engine/common.hpp"
#include "engine/things/settings.hpp"
#include "engine/theatre/g_theatre.hpp"
#include "engine/physics/g_jolt.hpp"
// Note: these includes are for when I move the J_CreateAShape function into here and merge it with Collider
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "math/gmath.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

//
// Collider
//
Collider::~Collider()
{
    J_RemoveAndDestroyBody(body_id);
}

void Collider::loadSettings()
{
    gSettings::configureBaseVariables(this);

    settings.getVariable("MotionType", motion_type);
    settings.getVariable("ObjectLayer", object_layer);
    settings.getVariable("Activation", activation);
    settings.getVariable("Shape", shape);
    settings.getBoolean("ForeverAlone", forever_alone);
    settings.getNumber("Position", position);
    settings.getNumber("Rotation", euler_angles);
    settings.getNumber("LocalPosition", local_position);
    settings.getNumber("LocalRotation", local_euler_angles);
    settings.getNumber("Scale", scale);
}

JPH::BodyCreationSettings* Collider::getBodySettings()
{
    return &body_settings;
}

const JPH::BodyID& Collider::getBodyID()
{
    return body_id;
}

void Collider::createBody()
{
    shape_arguments = std::make_tuple(scale, glm::max(glm::max(scale[0], scale[1]), scale[2]), scale[1]);
    JPH::RVec3 body_position = gmath::convertMath<JPH::Vec3>(position) + gmath::convertMath<JPH::Vec3>(local_position);
    JPH::Quat body_quaternion = JPH::Quat::sEulerAngles(gmath::convertMath<JPH::Vec3>(glm::radians(euler_angles))) * JPH::Quat::sEulerAngles(gmath::convertMath<JPH::Vec3>(glm::radians(local_euler_angles)));

    body_settings = JPH::BodyCreationSettings(J_CreateAShape(shape, shape_arguments), body_position, body_quaternion, motion_type, object_layer);
    body_id = jolt_physics_system.GetBodyInterface().CreateAndAddBody(body_settings, activation);
    jolt_physics_system.GetBodyInterface().SetFriction(body_id, friction);
}

void Collider::destroyBody()
{
    if(!body_id.IsInvalid())
        J_RemoveAndDestroyBody(body_id);
}

void Collider::initialize()
{
    if(forever_alone)
        createBody();
}

void Collider::prepForDestruction()
{
    Device::prepForDestruction();

    if(!body_id.IsInvalid() && jolt_physics_system.GetBodyInterface().IsAdded(body_id))
    {
        jolt_physics_system.GetBodyInterface().RemoveBody(body_id);
        jolt_physics_system.GetBodyInterface().DestroyBody(body_id);
    }
}

//
// Environment
//

void Environment::loadSettings()
{
    gSettings::configureBaseVariables(this);

    bool ambient_light_enabled = (ambient_light_amount > 0.0f);

    settings.getNumber("AmbientLightAmount", ambient_light_amount);
    settings.getNumber("AmbientLightColor", ambient_light_color);
    settings.getBoolean("AmbientLightEnabled", ambient_light_enabled);

    ambient_light_amount *= ambient_light_enabled;
}

//
// Texture
//
Texture::Texture(const std::string& init_name)
: Device(init_name)
{}

Texture::Texture(std::vector<unsigned char*> init_texture_data, std::vector<unsigned int> init_texture_size)
: Device("Untitled Texture"), texture_data(init_texture_data), texture_size(init_texture_size)
{}

Texture::Texture(unsigned char* init_texture_data, unsigned int init_texture_size)
: Texture(std::vector<unsigned char*>{init_texture_data}, std::vector<unsigned int>{init_texture_size})
{}

void Texture::loadSettings()
{
    gSettings::configureBaseVariables(this);
}

//
// Material
//
Material::Material(const bool use_missing)
: Device("Missing Material"), specular_strength(0.0f)
{
    if(use_missing)
    {
        mat_fullbright = true;
        diffuse_texture_name = MISSING_TEXTURE;
    }
}

Material::Material(glm::vec3 init_color, bool is_fullbright)
: Device("Untitled Material"), color(init_color), specular_strength(0.0f), mat_fullbright(is_fullbright)
{}

Material::Material(std::string init_name, std::string init_diffuse_texture_name, const bool is_fullbright, glm::vec3 init_color, std::string init_specular_texture_name, float init_specular_strength, int init_specular_sharpness)
: Device(init_name), diffuse_texture_name(init_diffuse_texture_name), specular_texture_name(init_specular_texture_name), color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
{}

// Material::Material(bool is_fullbright, glm::vec3 init_color)
// : Device("Untitled Material"), color(init_color), specular_strength(0.0f), mat_fullbright(is_fullbright)
// {}

// Material::Material(std::string init_diffuse_texture_name, std::string init_specular_texture_name, int init_specular_sharpness, float init_specular_strength, glm::vec3 init_color)
// : Device("Untitled Material"), diffuse_texture_name(init_diffuse_texture_name), specular_texture_name(init_specular_texture_name), color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
// {}

// Material::Material(glm::vec3 init_color, float init_specular_strength, unsigned int init_specular_sharpness)
// : Device("Untitled Material"), color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
// {}

void Material::loadSettings()
{
    gSettings::configureBaseVariables(this);

    settings.getResource("DiffuseTexture", diffuse_texture_name);
    settings.getResource("SpecularTexture", specular_texture_name);
    settings.getNumber("Color", color);
    settings.getNumber("Alpha", color_alpha);
    settings.getNumber("SpecularSharpness", specular_sharpness);
    settings.getNumber("SpecularStrength", specular_strength);
    settings.getBoolean("mat_fullbright", mat_fullbright);

    if(mat_fullbright && diffuse_texture_name == MISSING_TEXTURE)
        diffuse_texture_name = NO_TEXTURE;

    if(specular_texture_name == NO_TEXTURE)
        specular_strength = 0.0f;
}

//
// Mesh
//
Mesh::Mesh(const std::string& my_mesh_data_name, Material* my_material)
: Device("Untitled Model"), material(my_material), mesh_data_name(my_mesh_data_name)
{}

Mesh::Mesh(std::string init_mesh_data_name)
: Device("Untitled Mesh"), mesh_data_name(init_mesh_data_name)
{}

void Mesh::prepForDestruction()
{
    Device::prepForDestruction();

    if(material != nullptr)
        material->prepForDestruction();

    material = nullptr;
    delete material;
}

void Mesh::loadSettings()
{
    gSettings::configureBaseVariables(this);

    settings.getDevice("Material", material);
    settings.getResource("MeshData", mesh_data_name);
}

//
// Sprite
//
void Sprite::loadSettings()
{
    gSettings::configureBaseVariables(this);
    // Todo: expand configureBaseVariables to include other types (like Mesh)
    settings.getDevice("Material", material);
    mesh_data_name = GRAPHX_QUAD; // Override
}