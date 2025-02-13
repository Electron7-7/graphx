#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
#include "g_common.hpp"
#include "g_jolt.hpp"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Character/Character.h>
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
	Collider *collider = new Collider();

	float mass = 1.0f; // in kg

	PhysicsActor(std::string init_name = "Untitled Physics Actor", Mesh *init_mesh = new Mesh(), glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_euler_degrees = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(1.0f));

	void tick(int current_tick) override;
	void youGotACallBack(gSettings new_settings = null_settings) override;
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
	using PhysicsActor::PhysicsActor;

	void tick(int current_tick) override;
	void youGotACallBack(gSettings new_settings = null_settings) override;
	void callToStage(Theatre *parent_theatre) override;

	void reset_to_initial_orientation_for_testing() override;

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
	void youGotACallBack(gSettings new_settings = null_settings) override;
	void doRotation(glm::vec2 mouse_input);
};

/*class PlayerCamera : public Camera
{
public:
	PlayerCamera();

	void tick(int current_tick) override;
	void doRotation(glm::vec2 mouse_input);
};*/

/*class CharacterController: public Actor
{
public:

};*/

class GraphXPlayer: public Actor //public CharacterController
{
public:
	Mesh player_mesh = Mesh();
	Camera player_camera;
	Collider player_collider;
	Collider physics_collider;

	float mouse_sensitivity = 0.05f;
	float movement_speed = 0.1f;
	float max_velocity = 8.0f;

	JPH::Ref<JPH::CharacterSettings> player_settings;

	GraphXPlayer(std::string new_nam = "DEFAULT PLAYER", glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_rotation_euler = glm::vec3(0.0f, 0.0f, 0.0f));

	glm::mat4 getViewMatrix();
	void doMouseMovement(glm::vec2 mouse_offset);
	void doMovement(int direction[2]);
	bool wantsToBeRendered() override;
	void tick(int current_tick) override;
	void youGotACallBack(gSettings new_settings = null_settings) override;
	void callToStage(Theatre *parent_theatre) override;

private:
	JPH::Ref<JPH::Character> jph_character;
};

class Light: public Actor
{
public:
	Mesh temporary_light_mesh = Mesh(new Material(TOOL_TEXTURE_LIGHT, TOOL_TEXTURE_LIGHT, 0, 0.0f));

	unsigned int light_type;
	glm::vec3 light_color = glm::vec3(1.0f);
	float light_strength = 1.0f; // A more direct "brightness" value than just changing Attenuation values

	// Attenuation values
	float range = 100.0f;
	float intensity = 1.0f;	// negative scale: 0.0 is brightest and it gets dimmer as it increases
	float falloff = 0.0f;		// increasing causes light to fade more quickly with distance (multiplied by 0.01 in shader)

	Light(std::string init_name = "UNTITLED LIGHT", float init_intensity = 1.0f, float init_range = 100.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f), glm::vec3 init_position = glm::vec3(1.0f), glm::vec3 init_rotation = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(0.5f));

	void youGotACallBack(gSettings new_settings = null_settings) override;
};

class LightDirectional: public Light
{
public:
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);

	LightDirectional(std::string init_name = "UNTITLED DIRECTIONAL LIGHT", glm::vec3 init_direction = glm::vec3(0.0f, -1.0f, 0.0f), float init_strength = 0.4f, glm::vec3 init_color = glm::vec3(1.0f));

	void youGotACallBack(gSettings new_settings = null_settings) override;
};

class LightSpot: public Light
{
public:
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
	float inner_cutoff_angle = 12.5f;
	float outer_cutoff_angle = 17.5f;

	LightSpot(std::string init_name = "UNTITLED SPOT LIGHT", float init_intensity = 1.0f, float init_range = 100.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f), float init_inner_cutoff_angle = 12.5f, float init_outer_cutoff_angle = 17.5f, glm::vec3 init_direction = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3 init_position = glm::vec3(1.0f), glm::vec3 init_rotation = glm::vec3(0.0f));

	glm::vec2 getCutoffAngles();

	void youGotACallBack(gSettings new_settings = null_settings) override;
};

class LightFlashlight: public LightSpot
{
public:
	Actor *parent = NULL;
	glm::vec3 position_offset = glm::vec3(0.0f);
	glm::vec3 rotation_offset = glm::vec3(0.0f);

	LightFlashlight(std::string init_name = "UNTITLED FLASHLIGHT", float init_intensity = 0.5f, float init_range = 325.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f), float init_inner_cutoff_angle = 12.5f, float init_outer_cutoff_angle = 17.5f, glm::vec3 init_position_offset = glm::vec3(0.0f), glm::vec3 init_rotation_offset = glm::vec3(0.0f));

	void setLight(bool is_off);

	void tick(int current_tick) override;
	void youGotACallBack(gSettings new_settings = null_settings) override;

private:
	float _intensity;
};

class LightTesterMover : public Light
{
public:
	glm::vec3 pivot_position = glm::vec3(0.0f);
	float pivot_radius = 3.0f;
	float pivot_speed = 1.0f;
	float pivot_theta = 0.0f;

	Mesh temporary_pivot_mesh = Mesh(new Material(true, glm::vec3(1.0f, 0.0f, 0.0f)));
	Actor pivot_point = Actor("pivot point", &temporary_pivot_mesh, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.2f));

	LightTesterMover(std::string init_name = "UNTITLED MOVING LIGHT TESTER", glm::vec3 init_pivot_position = glm::vec3(0.0f), float init_pivot_radius = 3.0f, float init_pivot_speed = 1.0f, float init_intensity = 1.0f, float init_range = 325.0f, float init_falloff = 0.0f, float init_strength = 1.0f, glm::vec3 init_color = glm::vec3(1.0f));

	void tick(int current_tick) override;
	void youGotACallBack(gSettings new_settings = null_settings) override;
	void callToStage(Theatre *parent_theatre) override;
};

extern GraphXPlayer *current_player;
extern std::unordered_map<double, Actor *> actor_uid_lookup;
extern glm::vec3 vector3_up;
extern glm::vec3 vector3_front;
extern glm::vec3 vector3_right;
#endif