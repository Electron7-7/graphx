#include "g_devices.hpp"
#include "graphx_namespace.hpp"
#include "t_settings.hpp"
#include "g_theatre.hpp"
#include "g_jolt.hpp"
// Note: these includes are for when I move the J_CreateAShape function into here and merge it with Collider
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include <gmath.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

//
// Collider
//
Collider::~Collider()
{
    J_RemoveAndDestroyBody(body_id);
}

void Collider::loadSettings(graphx::gSettings new_settings)
{
    Device::loadSettings(new_settings);

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

    if(jolt_physics_system.GetBodyInterface().IsAdded(body_id))
        J_RemoveAndDestroyBody(body_id);
}

//
// Environment
//

void Environment::loadSettings(graphx::gSettings new_settings)
{
    Device::loadSettings(new_settings);

    bool ambient_light_enabled = (ambient_light_amount > 0.0f);

    getSetting(ambient_light_amount, settings["AmbientLightAmount"]);
    getSetting(ambient_light_color, settings["AmbientLightColor"]);
    getSetting(ambient_light_enabled, settings["AmbientLightEnabled"]);

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

void Texture::loadSettings(graphx::gSettings new_settings)
{
    Device::loadSettings(new_settings);
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

// Todo: Use the bottom two instead of the rest of the constructors
// Material::Material(glm::vec3 init_color, bool is_fullbright)
// : Device("Untitled Material"), color(init_color), specular_strength(0.0f), mat_fullbright(is_fullbright)
// {}

// Material::Material(std::string init_name, std::string init_diffuse_texture_name, const bool is_fullbright, glm::vec3 init_color, std::string init_specular_texture_name, float init_specular_strength, int init_specular_sharpness)
// : Device(init_name), diffuse_texture_name(init_diffuse_texture_name), specular_texture_name(init_specular_texture_name), color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
// {}

Material::Material(bool is_fullbright, glm::vec3 init_color)
: Device("Untitled Material"), color(init_color), specular_strength(0.0f), mat_fullbright(is_fullbright)
{}

Material::Material(std::string init_diffuse_texture_name, std::string init_specular_texture_name, int init_specular_sharpness, float init_specular_strength, glm::vec3 init_color)
: Device("Untitled Material"), diffuse_texture_name(init_diffuse_texture_name), specular_texture_name(init_specular_texture_name), color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
{}

Material::Material(glm::vec3 init_color, float init_specular_strength, unsigned int init_specular_sharpness)
: Device("Untitled Material"), color(init_color), specular_sharpness(init_specular_sharpness), specular_strength(init_specular_strength)
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
// Mesh
//
Mesh::Mesh(const std::string& my_mesh_data_name, const int my_material_uid)
: Device("Untitled Model")/*, material_uid(my_material_uid)*/, mesh_data_name(my_mesh_data_name)
{}

Mesh::Mesh(Material *new_material, std::string init_mesh_data_name)
: Device("Untitled Mesh"), material(new_material), mesh_data_name(init_mesh_data_name)
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

void Mesh::loadSettings(graphx::gSettings new_settings)
{
    Device::loadSettings(new_settings);

    getSetting(material, settings["Material"]);
    getSetting(mesh_data_name, settings["MeshData"]);
}

//
// Sprite
//
void Sprite::loadSettings(graphx::gSettings new_settings)
{
    Mesh::loadSettings(new_settings);
    mesh_data_name = GRAPHX_QUAD; // Override
}