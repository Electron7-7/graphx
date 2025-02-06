#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
#include "g_common.hpp"
#include "g_jolt.hpp"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/EmptyShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

#define ACTOR_ACTOR 		0
#define ACTOR_LIGHT 		1
#define ACTOR_PHYSICS		2
#define ACTOR_PLAYER		3

#define LIGHT_POINT			0
#define LIGHT_DIRECTIONAL	1
#define LIGHT_SPOT			2

class PhysicsActor: public Actor
{
public:
	std::vector<JPH::BodyID> collider_ids;

	float mass = 1.0f; // in kg

	PhysicsActor(std::string init_name, JPH::BodyCreationSettings init_body_creation_settings, JPH::EActivation init_body_activation = JPH::EActivation::Activate, Mesh *init_mesh = NULL, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_euler_degrees = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(1.0f));
	PhysicsActor(std::string init_name, JPH::EMotionType init_motion_type, JPH::ObjectLayer init_object_layer, JPH::EActivation init_body_activation = JPH::EActivation::Activate, Mesh *init_mesh = NULL, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_euler_degrees = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(1.0f));

	void tick(int current_tick) override;
	void callToStage(Theatre *parent_theatre) override;
	void takeABow() override;

	virtual void reset_to_initial_orientation_for_testing()
	{};

protected:
	std::vector<JPH::BodyCreationSettings> body_creation_settings;
	std::vector<JPH::EActivation> body_activation;
	JPH::Vec3 body_scale;
	JPH::Vec3 body_position;
	JPH::Quat body_quat;
	JPH::EActivation test_body_activation;
	JPH::EMotionType test_motion_type;
	JPH::ObjectLayer test_object_layer;
};

class RigidBodyActor : public PhysicsActor
{
public:
	int controller_collider_index = 0;

	using PhysicsActor::PhysicsActor;

	void reset_to_initial_orientation_for_testing() override;
	void tick(int current_tick) override;
	void callToStage(Theatre *parent_theatre) override;

protected:
	JPH::Vec3 reset_position;
	JPH::Quat reset_quaternion;
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

	void tick(int current_tick) override;
	void doRotation(glm::vec2 mouse_input);
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

	GraphXPlayer(std::string new_name, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_rotation_euler = glm::vec3(0.0f, 0.0f, 0.0f));

	glm::mat4 getViewMatrix();
	void doMouseMovement(glm::vec2 mouse_offset);
	void doMovement(int direction[2]);
	bool wantsToBeRendered() override;
	void callToStage(Theatre *parent_theatre) override;
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

	Light(std::string init_name, float init_intensity = 1.0f, float init_range = 100.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f), glm::vec3 init_position = glm::vec3(1.0f), glm::vec3 init_rotation = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(0.5f));
};

class LightDirectional: public Light
{
public:
	glm::vec3 direction;

	LightDirectional(std::string init_name, glm::vec3 init_direction = glm::vec3(0.0f, -1.0f, 0.0f), float init_strength = 0.4f, glm::vec3 init_color = glm::vec3(1.0f));
};

class LightSpot: public Light
{
public:
	glm::vec3 direction;
	float inner_cutoff_angle;
	float outer_cutoff_angle;

	LightSpot(std::string init_name, float init_intensity = 1.0f, float init_range = 100.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f), float init_inner_cutoff_angle = 12.5f, float init_outer_cutoff_angle = 17.5f, glm::vec3 init_direction = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3 init_position = glm::vec3(1.0f), glm::vec3 init_rotation = glm::vec3(0.0f));

	glm::vec2 getCutoffAngles();
};

class LightFlashlight: public LightSpot
{
public:
	Actor *parent = NULL;
	glm::vec3 position_offset;
	glm::vec3 rotation_offset;

	LightFlashlight(std::string init_name, float init_intensity = 0.5f, float init_range = 325.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f), float init_inner_cutoff_angle = 12.5f, float init_outer_cutoff_angle = 17.5f, glm::vec3 init_position_offset = glm::vec3(0.0f), glm::vec3 init_rotation_offset = glm::vec3(0.0f));

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

	LightTesterMover(std::string init_name, glm::vec3 init_pivot_position, float init_pivot_radius, float init_pivot_speed = 1.0f, float init_intensity = 1.0f, float init_range = 325.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f));

	void tick(int current_tick) override;
	void callToStage(Theatre *parent_theatre) override;
};

extern GraphXPlayer *current_player;
extern std::unordered_map<double, Actor *> actor_uid_lookup;
extern glm::vec3 vector3_up;
extern glm::vec3 vector3_front;
extern glm::vec3 vector3_right;
#endif