#ifndef GRAPHX_DEVICES
#include "g_jolt.hpp"
#include "g_device.hpp"
#include <images.h>
#include <models.hpp>
#define GRAPHX_DEVICES

struct Collider : public Device
{
    glm::vec3           local_position = glm::vec3(0.0f);
    glm::vec3           position = glm::vec3(0.0f);
    glm::vec3           euler_angles = glm::vec3(0.0f);
    glm::vec3           local_euler_angles = glm::vec3(0.0f);
    glm::vec3           scale = glm::vec3(1.0f);
    JPH::EMotionType    motion_type = JPH::EMotionType::Dynamic;
    JPH::ObjectLayer    object_layer = Layers::MOVING;
    JPH::EActivation    activation = JPH::EActivation::Activate;
    float               friction = 1.0f;
    bool                forever_alone = false;
    int                 shape = graphx::jolt::shapes::BOX;

    graphx::jolt::shape_arguments shape_arguments;

    using Device::Device;
    ~Collider() override;

    void createBody();
    void destroyBody();

    const JPH::BodyID &getBodyID();
    JPH::BodyCreationSettings *getBodySettings();

    void loadSettings(graphx::gSettings new_settings = empty_settings) override;
    void initialize() override;
    void prepForDestruction() override;

protected:
    JPH::BodyID body_id;
    JPH::BodyCreationSettings body_settings;
};

struct Environment final : public Device // Will be extended in the future
{
    glm::vec3 ambient_light_color = glm::vec3(1.0f);
    float ambient_light_amount = 0.05f;

    using Device::Device;
    // Environment(float = 0.05f, glm::vec3 = glm::vec3(1.0f));

    void loadSettings(graphx::gSettings new_settings = empty_settings) override;
};

struct Texture final : public Device
{
public:
    bool is_in_use = false;
    unsigned int texture_id = 0;
    std::vector<unsigned char *> texture_data = {MISSING_TEXTURE_jpg};
    std::vector<unsigned int> texture_size = {MISSING_TEXTURE_jpg_len};

    using Device::Device;
    Texture(const std::string& = "Untitled Texture"); // Todo: I only need to have this here because of the map of textures...
    Texture(unsigned char*, unsigned int);
    Texture(std::vector<unsigned char*>, std::vector<unsigned int>);
    // Texture(std::vector<unsigned char *> init_texture_data, std::vector<unsigned int> init_texture_size);
    // Texture(std::vector<const char *> init_texture_data, std::vector<unsigned int> init_texture_size);
    // Texture(std::vector<std::string > init_texture_data, std::vector<unsigned int> init_texture_size);
    // Texture(unsigned char * init_texture_data, unsigned int init_texture_size);
    // Texture(const char * init_texture_data, unsigned int init_texture_size);
    // Texture(std::string  init_texture_data, unsigned int init_texture_size);

    void loadSettings(graphx::gSettings new_settings = empty_settings) override;
};

struct Material final : public Device
{
    std::string diffuse_texture_name = MISSING_TEXTURE;
    std::string specular_texture_name = MISSING_TEXTURE;

    glm::vec3 color = glm::vec3(1.0f);
    float color_alpha = 1.0f;
    int specular_sharpness = 16;
    float specular_strength = 1.0f;
    bool mat_fullbright = false;
    bool use_texture = true;

    using Device::Device;
    Material(const bool USE_MISSING_INSTEAD_OF_EMPTY = false);
    // Material(glm::vec3 Color, bool Fullbright = false);
    // Material(std::string MaterialName, std::string DiffuseTextureName, const bool Fullbright, glm::vec3 Color = glm::vec3(1.0f), std::string SpecularTextureName = NO_TEXTURE, float SpecularStrength = 0.0f, int SpecularSharpness = 16);
    // REPLACE THE BOTTOM THREE WITH THE TOP TWO
    Material(bool is_fullbright, glm::vec3 init_color = glm::vec3(1.0f));
    Material(std::string init_diffuse_texture_name, std::string init_specular_texture_name = NO_TEXTURE, int init_specular_sharpness = 16, float init_specular_strength = 0.0f, glm::vec3 init_color = glm::vec3(1.0f));
    Material(glm::vec3 init_color, float init_specular_strength = 0.5f, unsigned int init_specular_sharpness = 32);

    void loadSettings(graphx::gSettings new_settings = empty_settings) override;
};

struct Mesh : public Device
{
    Material *material = new Material();

    unsigned int VBO = 0;
    unsigned int IBO = 0;
    bool is_buffered = false;
    glm::vec3 mesh_scale = glm::vec3(1.0f);
    std::string mesh_data_name = ERROR_MODEL;

    using Device::Device;
    Mesh(const std::string& MeshDataName = ERROR_MODEL, const int MaterialUID = -1);
    Mesh(Material *new_material, std::string init_mesh_data_name = ERROR_MODEL);
    Mesh(std::string init_mesh_data_name);

    void loadSettings(graphx::gSettings new_settings = empty_settings) override;
    void prepForDestruction() override;
};

// Differentiating 3D meshes and 2D sprites, even though they're extremely similar (for sanity reasons)
struct Sprite : public Mesh
{
    // Sprite(std::string init_name = "UNTITLED_SPRITE");
    using Mesh::Mesh;

    void loadSettings(graphx::gSettings new_settings = empty_settings) override;
};
#endif