#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
#include "r_common.hpp"
#include "g_common.hpp"
#include "g_math.hpp"
#include <vector>

#define LIGHT_POINT				0
#define LIGHT_DIRECTIONAL		1
#define LIGHT_SPOT				2

class PhysicsActor: public Actor
{
public:
	float mass = 1.0f; // in kg
	float drag = 0.0f;
	float friction = 0.0f;

	JPH::BodyID physics_body_id;
	JPH::BodyCreationSettings box_settings;
	JPH::PhysicsSystem *physics_system = NULL;

	glm::vec3 reset_position;
	JPH::Quat reset_quaternion;

	PhysicsActor(std::string new_name, Mesh init_mesh, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_rotation_euler = glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3 init_scale = glm::vec3(1.0f))
	: Actor(new_name, init_mesh, init_position, init_rotation_euler, init_scale), reset_position(init_position), reset_quaternion(JPH::Quat::sEulerAngles(convertMath<JPH::Vec3>(init_rotation_euler)))
	{ actor_type = ACTOR_PHYSICS; }

	void init(Theatre *parent_theatre) override;
	void Tick(int current_tick) override;
	void reset_to_initial_orientation_for_testing();
};

class GraphXPlayer: public Actor
{
public:
	float mouse_sensitivity;
	float movement_speed = 0.05f;

	JPH::BodyID physics_body_id;
	JPH::BodyCreationSettings box_settings;
	JPH::PhysicsSystem *physics_system = NULL;

	GraphXPlayer(std::string new_name, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_rotation_euler = glm::vec3(0.0f, -90.0f, 0.0f))
	: Actor(new_name, Mesh(), init_position, init_rotation_euler), mouse_sensitivity(INIT_SENSITIVITY)
	{
		actor_type = ACTOR_PLAYER;
		visible = false;
		debug_visible = false;
	}

	glm::mat4 getViewMatrix();
	void doMouseMovement(std::vector<float> offset, bool constrain_pitch = true);
	void doMovement(int direction[2]);
	bool wantsToBeRendered() override;
	void init(Theatre *parent_theatre) override;
	void Tick(int current_tick) override;

protected:
	constexpr static const float INIT_SENSITIVITY = 0.1f;
};

class MoverTester: public Actor
{
public:
	float movement_speed = 0.025f;
	int t_direction = 0;

	MoverTester(std::string init_name, Mesh init_mesh, glm::vec3 init_position = glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3 init_rotation = glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3 init_scale = glm::vec3(1.0f))
	: Actor(init_name, init_mesh, init_position, init_rotation, init_scale)
	{}

	void Tick(int current_tick) override;
};

class ControlledTester: public PhysicsActor
{
public:
	float movement_speed = 0.025f;
	int movement_direction[3];

	ControlledTester(std::string init_name, Mesh init_mesh, glm::vec3 init_position = glm::vec3(0.0f, 3.0f, -3.0f), glm::vec3 init_rotation_euler = glm::vec3(0.0f, -90.0f, 0.0f), glm::vec3 init_scale = glm::vec3(1.0f))
	: PhysicsActor(init_name, init_mesh, init_position, init_rotation_euler, init_scale)
	{}

	void Tick(int current_tick) override;
};

class Light: public Actor
{
public:
	unsigned int light_type;
	glm::vec3 light_color;
	float light_strength; // A more direct "brightness" value than just changing Attenuation values

	// Attenuation values
	float range;
	float intensity;	// negative scale: 0.0 is brightest and it gets dimmer as it increases
	float falloff;		// increasing causes light to fade more quickly with distance (multiplied by 0.01 in shader)

	Light(std::string init_name, float init_intensity = 1.0f, float init_range = 100.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f), glm::vec3 init_position = glm::vec3(1.0f), glm::vec3 init_rotation = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(0.5f))
	: Actor(init_name, Mesh(Material(TOOL_TEXTURE_LIGHT, TOOL_TEXTURE_LIGHT, 0, 0.0f)), init_position, init_rotation, init_scale), light_color(init_color), light_strength(init_strength), range(init_range), intensity(init_intensity), falloff(init_falloff)
	{
		actor_type = ACTOR_TOOL;
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
	void Tick(int current_tick) override;

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
	Actor pivot_point;

	LightTesterMover(std::string init_name, glm::vec3 init_pivot_position, float init_pivot_radius, float init_pivot_speed = 1.0f, float init_intensity = 1.0f, float init_range = 325.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f))
	: Light(init_name, init_intensity, init_range, init_falloff, init_strength, init_color), pivot_position(init_pivot_position), pivot_radius(init_pivot_radius), pivot_speed(init_pivot_speed), pivot_point(Actor(std::string("pivot point for ") + init_name, Mesh(Material(true, glm::vec3(1.0f, 0.0f, 0.0f))), init_pivot_position, glm::vec3(0.0f), glm::vec3(0.2f)))
	{
		pivot_point.mesh.name = "PIVOT";
	}

	void Tick(int current_tick) override;
	void init(Theatre *parent_theatre) override;
};

extern GraphXPlayer *current_player;
#endif