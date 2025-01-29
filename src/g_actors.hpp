#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
#include "r_common.hpp"
#include "g_common.hpp"
// #include <Jolt/Physics/Character/Character.h>

#define LIGHT_POINT				0
#define LIGHT_DIRECTIONAL		1
#define LIGHT_SPOT				2

class PhysicsActor: public Actor
{
public:
	float mass = 1.0f; // in kg

	// JPH::BodyID physics_body_id;
	// JPH::BodyCreationSettings collider_settings;
	// JPH::PhysicsSystem *physics_system = NULL;

	// JPH::Vec3 reset_position;
	// JPH::Quat reset_quaternion;

	PhysicsActor(std::string new_name, Mesh *init_mesh, float init_mass = 1.0f, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_rotation_euler = glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3 init_scale = glm::vec3(1.0f))
	: Actor(new_name, init_mesh, init_position, init_rotation_euler, init_scale), mass(init_mass)
	{ actor_type = ACTOR_PHYSICS; }

	void init(Theatre *parent_theatre) override;
	void tick(int current_tick) override;
	void reset_to_initial_orientation_for_testing();
};

class Camera : public Actor
{
public:
	Actor *parent = NULL;
	float view_pitch_clamp = 89.0f;
	glm::vec3 position_global = glm::vec3(0.0f);
	glm::vec3 position_local = glm::vec3(0.0f, 3.0f, 0.0f); // temporary default offset
	glm::vec3 euler_rotation = glm::radians(glm::vec3(0.0f, -90.0f, 0.0f));
	glm::vec3 euler_rotation_local = glm::vec3(0.0f);

	void doRotation(glm::vec2 mouse_input);
	void tick(int current_tick);
};

class GraphXPlayer: public Actor
{
public:
	Mesh player_mesh = Mesh();
	Camera player_camera;

	float mouse_sensitivity = 0.05f;
	float movement_speed = 0.1f;
	float max_velocity = 8.0f;

	// JPH::BodyID physics_body_id;
	// JPH::BodyID gravity_body_id;
	// JPH::PhysicsSystem *physics_system = NULL;
	// JPH::Ref<JPH::CharacterSettings> player_settings;

	GraphXPlayer(std::string new_name, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_rotation_euler = glm::vec3(0.0f, 0.0f, 0.0f))
	: Actor(new_name, &player_mesh, init_position, init_rotation_euler, glm::vec3(1.5f, 3.0f, 1.5f))
	{
		actor_type = ACTOR_PLAYER;
		debug_visible = false;
		player_camera.euler_rotation = glm::radians(init_rotation_euler);
		player_camera.position_global = init_position;
		player_camera.parent = this;
	}

	glm::mat4 getViewMatrix();
	void doMouseMovement(glm::vec2 mouse_offset);
	void doMovement(int direction[2]);
	bool wantsToBeRendered() override;
	void init(Theatre *parent_theatre) override;
	void tick(int current_tick) override;

/*private:
	JPH::Ref<JPH::Character> jph_character;*/
};

class Light: public Actor
{
public:
	Mesh temporary_light_mesh = Mesh(Material(TOOL_TEXTURE_LIGHT, TOOL_TEXTURE_LIGHT, 0, 0.0f));

	unsigned int light_type;
	glm::vec3 light_color;
	float light_strength; // A more direct "brightness" value than just changing Attenuation values

	// Attenuation values
	float range;
	float intensity;	// negative scale: 0.0 is brightest and it gets dimmer as it increases
	float falloff;		// increasing causes light to fade more quickly with distance (multiplied by 0.01 in shader)

	Light(std::string init_name, float init_intensity = 1.0f, float init_range = 100.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f), glm::vec3 init_position = glm::vec3(1.0f), glm::vec3 init_rotation = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(0.5f))
	: Actor(init_name, &temporary_light_mesh, init_position, init_rotation, init_scale), light_color(init_color), light_strength(init_strength), range(init_range), intensity(init_intensity), falloff(init_falloff)
	{
		actor_type = ACTOR_LIGHT;
		light_type = LIGHT_POINT;
		debug_visible = true;
	}
};

class LightDirectional: public Light
{
public:
	glm::vec3 direction;

	LightDirectional(std::string init_name, glm::vec3 init_direction = glm::vec3(0.0f, -1.0f, 0.0f), float init_strength = 0.4f, glm::vec3 init_color = glm::vec3(1.0f), glm::vec3 init_position = glm::vec3(1.0f), glm::vec3 init_rotation = glm::vec3(0.0f))
	: Light(init_name, 1.0f, 100.0f, 0.0f, init_strength, init_color, init_position, init_rotation, glm::vec3(0.0f)), direction(init_direction)
	{ light_type = LIGHT_DIRECTIONAL; }
};

class LightSpot: public Light
{
public:
	glm::vec3 direction;
	float inner_cutoff_angle;
	float outer_cutoff_angle;

	LightSpot(std::string init_name, float init_intensity = 1.0f, float init_range = 100.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f), float init_inner_cutoff_angle = 12.5f, float init_outer_cutoff_angle = 17.5f, glm::vec3 init_direction = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3 init_position = glm::vec3(1.0f), glm::vec3 init_rotation = glm::vec3(0.0f))
	: Light(init_name, init_intensity, init_range, init_falloff, init_strength, init_color, init_position, init_rotation), direction(init_direction), inner_cutoff_angle(init_inner_cutoff_angle), outer_cutoff_angle(init_outer_cutoff_angle)
	{ light_type = LIGHT_SPOT; }

	glm::vec2 getCutoffAngles();
};

class LightFlashlight: public LightSpot
{
public:
	Actor *parent = NULL;
	glm::vec3 position_offset;
	glm::vec3 rotation_offset;

	LightFlashlight(std::string init_name, float init_intensity = 0.5f, float init_range = 325.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f), float init_inner_cutoff_angle = 12.5f, float init_outer_cutoff_angle = 17.5f, glm::vec3 init_position_offset = glm::vec3(0.0f), glm::vec3 init_rotation_offset = glm::vec3(0.0f))
	: LightSpot(init_name, init_intensity, init_range, init_falloff, init_strength, init_color, init_inner_cutoff_angle, init_outer_cutoff_angle), position_offset(init_position_offset), rotation_offset(init_rotation_offset), _intensity(init_intensity)
	{
		light_type = LIGHT_SPOT;
		debug_visible = false;
	}

	void setLight(bool is_off);
	void tick(int current_tick) override;

private:
	float _intensity;
};

class LightTesterMover : public Light
{
public:
	glm::vec3 pivot_position;
	float pivot_radius;
	float pivot_speed;
	float pivot_theta = 0.0f;

	Mesh temporary_pivot_mesh = Mesh(Material(true, glm::vec3(1.0f, 0.0f, 0.0f)));
	Actor pivot_point = Actor("pivot point", &temporary_pivot_mesh, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.2f));

	LightTesterMover(std::string init_name, glm::vec3 init_pivot_position, float init_pivot_radius, float init_pivot_speed = 1.0f, float init_intensity = 1.0f, float init_range = 325.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f))
	: Light(init_name, init_intensity, init_range, init_falloff, init_strength, init_color), pivot_position(init_pivot_position), pivot_radius(init_pivot_radius), pivot_speed(init_pivot_speed)
	{
		pivot_point.name = "Pivot point Actor for " + name + " LightTesterMover (UID: " + std::to_string(UID) + ")";
		pivot_point.mesh->name = "Pivot Mesh for " + name + " LightTesterMover (UID: " + std::to_string(UID) + ")";
		pivot_point.position_global = init_pivot_position;
	}

	void tick(int current_tick) override;
	void init(Theatre *parent_theatre) override;
};

extern GraphXPlayer *current_player;
#endif