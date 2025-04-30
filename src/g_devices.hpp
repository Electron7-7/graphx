#ifndef GRAPHX_DEVICES
#include "graphx_namespace.hpp"
#include "g_jolt.hpp"
#include "g_device.hpp"
#include <images.h>
#include <models.hpp>
#define GRAPHX_DEVICES

struct Collider : public Device
{
	bool                overrides_actor_transform = true;
	// Defaults to a rigidbody type of body
	glm::vec3 			local_position = glm::vec3(0.0f);
	glm::vec3			position = glm::vec3(0.0f);
	glm::vec3			euler_angles = glm::vec3(0.0f);
	glm::vec3 			local_euler_angles = glm::vec3(0.0f);
	glm::vec3			scale = glm::vec3(1.0f);
	JPH::EMotionType	motion_type = JPH::EMotionType::Dynamic;
	JPH::ObjectLayer	object_layer = Layers::MOVING;
	JPH::EActivation	activation = JPH::EActivation::Activate;
	float				friction = 1.0f;
	bool				forever_alone = false;
	int 				shape = graphx::jolt::shapes::BOX;

	graphx::jolt::shape_arguments shape_arguments;

	using Device::Device;
	~Collider() override;

	JPH::BodyCreationSettings* getBodySettings();
	const JPH::BodyID& getBodyID();

	void reset_to_default_transformation_for_testing();
	void loadSettings() override;

protected:
	JPH::BodyID body_id;
	std::shared_ptr<JPH::Shape> body_shape = nullptr;
	JPH::BodyCreationSettings body_settings;
	JPH::Vec3 reset_position = JPH::Vec3(0.0f, 0.0f, 0.0f);
	JPH::Quat reset_quaternion = JPH::Quat::sIdentity();
	// A note about collider scale: it's not a simple scale value, as much as it's a complex shape; a scale value would affect the shape like a cube, which may work sometimes and may be strange other times
	JPH::Vec3 reset_scale = JPH::Vec3(1.0f, 1.0f, 1.0f);
};

struct Environment final : public Device // Will be extended in the future
{
	glm::vec3 ambient_light_color = glm::vec3(1.0f);
	float ambient_light_amount = 0.05f;

	using Device::Device;

	void loadSettings() override;
};

struct Texture final : public Device
{
public:
	bool is_in_use = false;
	unsigned int texture_id = 0;
	std::vector<unsigned char*> texture_data = {MISSING_TEXTURE_jpg};
	std::vector<unsigned int> texture_size = {MISSING_TEXTURE_jpg_len};

	using Device::Device;
	Texture(const std::string& = "Untitled Texture"); // Todo: I only need to have this here because of the map of textures...
	Texture(unsigned char*, unsigned int);
	Texture(std::vector<unsigned char*>, std::vector<unsigned int>);

	void loadSettings() override;
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
	Material(bool is_fullbright, glm::vec3 init_color = glm::vec3(1.0f));
	Material(std::string init_diffuse_texture_name, std::string init_specular_texture_name = NO_TEXTURE, int init_specular_sharpness = 16, float init_specular_strength = 0.0f, glm::vec3 init_color = glm::vec3(1.0f));
	Material(glm::vec3 init_color, float init_specular_strength = 0.5f, unsigned int init_specular_sharpness = 32);

	void loadSettings() override;
};

struct Model : public Device
{
	// int material_uid;
	Material material_base = Material(glm::vec3(1.0f)); // This is a bad way to make sure material is never nullptr
	std::shared_ptr<Material> material = std::make_shared<Material>(&material_base); // This is going to be replaced with material_id

	unsigned int VBO = 0;
	unsigned int IBO = 0;
	bool is_buffered = false;
	glm::vec3 mesh_scale = glm::vec3(1.0f);
	std::string mesh_data_name = ERROR_MODEL;

	using Device::Device;
	// Model(const int, const std::string& = ERROR_MODEL); // for when I replace Material* with int
	Model(const Material&, const std::string& = ERROR_MODEL);
	Model(std::shared_ptr<Material>, const std::string& = ERROR_MODEL);

	void loadSettings() override;
};

// Differentiating 3D meshes and 2D sprites, even though they're extremely similar (for sanity reasons)
struct Sprite : public Model
{
	using Model::Model;

	void loadSettings() override;
};

namespace graphx
{
	namespace current
	{
		extern std::shared_ptr<Environment> environment;
	}

	namespace safety
	{
		extern Environment environment;
	}
}
#endif