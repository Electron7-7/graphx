#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
#include "engine/things/devices/devices.hpp"
#include "engine/common.hpp"
#include "engine/rendering/common.hpp"
#include "engine/things/actors/actor.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Character/Character.h>

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
class LightFlashlight; // Forward Declaration
#endif

class Label : public Actor
{
public:
	Actor* parent = nullptr;
	float label_alpha = 0.0f;

	using Actor::Actor;

	RenderCommands getRenderCommands() override;
	void tick(int current_tick) override;
	void loadSettings() override;

protected:
	TextRenderCmd text_render_command;
};

class PhysicsActor: public Actor
{
public:
	std::unique_ptr<Collider> unique_collider = std::make_unique<Collider>("Empty Collider");
	Collider* collider = unique_collider.get();

	float mass = 1.0f; // in kg

	using Actor::Actor;

	void overrideColliderPosition(glm::vec3 Position);
	void overrideColliderRotation(glm::vec3 EulerRotation, bool AsDegrees);

	void tick(int current_tick) override;
	void loadSettings() override;
	void callToStage(Theatre *parent_theatre) override;
	void takeABow() override;

	virtual bool isPhysicsActor() const override final;
	virtual void reset_to_initial_orientation_for_testing();

protected:
	JPH::Vec3 reset_position = JPH::Vec3(0.0f, 0.0f, 0.0f);
	JPH::Quat reset_quaternion = JPH::Quat::sIdentity();
	std::vector<JPH::BodyCreationSettings> body_creation_settings;
	std::vector<JPH::EActivation> body_activation;
	JPH::Vec3 body_scale = JPH::Vec3(1.0f, 1.0f, 1.0f);
	JPH::Vec3 body_position = JPH::Vec3(0.0f, 0.0f, 0.0f);
	JPH::Quat body_quat = JPH::Quat::sIdentity();
	JPH::EActivation test_body_activation;
	JPH::EMotionType test_motion_type;
	JPH::ObjectLayer test_object_layer;
};

class RigidBodyActor : public PhysicsActor
{
public:
	using PhysicsActor::PhysicsActor;

	void tick(int current_tick) override;
	void loadSettings() override;
	void callToStage(Theatre *parent_theatre) override;
	void takeABow() override;

	void reset_to_initial_orientation_for_testing() override;
};

class StaticBodyActor : public PhysicsActor
{
public:
	using PhysicsActor::PhysicsActor;

	void loadSettings() override;
	void callToStage(Theatre *parent_theatre) override;
	void takeABow() override;
};

class Camera : public Actor
{
public:
	float view_pitch_clamp = 89.0f;
	glm::vec3 euler_rotation = glm::radians(glm::vec3(0.0f, -90.0f, 0.0f));
	glm::vec3 euler_rotation_local = glm::vec3(0.0f);

	using Actor::Actor;

	void tick(int current_tick) override;
	void loadSettings() override;
	void doRotation(glm::vec2 mouse_input);

protected:
	glm::vec3 position_global = glm::vec3(0.0f);
	glm::vec3 position_local = glm::vec3(0.0f, 3.0f, 0.0f); // temporary default offset
};

class GraphXPlayer: public Actor //public CharacterController(?)
{
public:
	Camera player_camera = Camera("Camera");
	LightFlashlight* player_flashlight = nullptr;

	bool do_gravity = true; // Debugging, mostly
	float mouse_sensitivity = 0.05f;
	float movement_speed = 13.0f;
	double lerp_speed = 1.3f;
	float friction = 0.85f;
	float mass = 100.0f;
	float field_of_view = 45.0f; // Make sure to convert to radians when making the perspective matrix 

	JPH::Ref<JPH::CharacterSettings> player_settings;

	using Actor::Actor;

	glm::mat4 getViewMatrix();
	glm::vec3 getViewPosition();
	void processMouse(GLFWwindow* window, double x_position_in, double y_position_in) override;
	void checkForInput(GLFWwindow* window) override;
	void processKey(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void doMouseMovement(glm::vec2 mouse_offset);
	void doMovement(int direction[2]);
	void tick(int current_tick) override;
	void loadSettings() override;
	void callToStage(Theatre *parent_theatre) override;
	void takeABow() override;

private:
	glm::vec3 flashlight_debug_toggle_color_god_damn_this_variable_name_is_long = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec2 mouse_last = glm::vec2(0.0f);
	JPH::Ref<JPH::Character> jph_character = nullptr;
	double movement_lerp = 0.0f;
	int last_direction[2] = {0, 0};
};

class Light : public Actor
{
public:
	// Basic Properties
	glm::vec3 light_color = glm::vec3(1.0f);
	float light_specular_strength = 1.0f;
	float light_ambient_strength = 0.05f;
	float light_energy = 0.8f;

	// Attenuation
	float light_attenuation = 1.0f; // Keep this between 0 and 10 for best results. Setting it to 2 results in the proper inverse square attenuation (for physically accurate lights/realism)
	float light_range = 100.0f;

	// Other
	// Texture *texture_projection = nullptr;

	using Actor::Actor;

	RenderCommands getRenderCommands() override;
	void loadSettings() override;

protected:
#ifdef GRAPHX_DEBUG
	bool debug_visible = true;
#else
	bool debug_visible = false;
#endif
};

class LightDirectional : public Light
{
public:
	glm::vec3 directional_direction = glm::vec3(0.0f, -0.7f, 0.2f); // Funny name

	using Light::Light;

	RenderCommands getRenderCommands() override;
	void loadSettings() override;
};

class LightSpot : public Light
{
public:
	glm::vec3 spot_direction = glm::vec3(0.1f, -0.5f, 0.8f);
	float spot_angle = 17.5f;
	float spot_angle_fade = 5.0f;

	using Light::Light;

	RenderCommands getRenderCommands() override;
	void loadSettings() override;
};

class LightFlashlight: public LightSpot
{
public:
	GraphXPlayer *parent = nullptr;
	glm::vec3 position_offset = glm::vec3(0.0f);
	glm::vec3 rotation_offset = glm::vec3(0.0f);
	bool start_enabled = true;

	using LightSpot::LightSpot;

	void setLight(bool is_on);
	void toggleLight(glm::vec3 toggle_color = glm::vec3(0.0f));
	void setLightColor(glm::vec3 color);
	void setLightColor(bool color_toggle);

	void tick(int current_tick) override;
	void loadSettings() override;

private:
	glm::vec3 _color = light_color;
};

class LightTesterMover : public Light
{
public:
	glm::vec3 pivot_position = glm::vec3(0.0f);
	float pivot_radius = 3.0f;
	float pivot_speed = 1.0f;
	float pivot_theta = 0.0f;

	Material temporary_pivot_material = Material(glm::vec3(1.0f, 0.0f, 0.0f), true);
	Mesh temporary_pivot_mesh = Mesh(GRAPHX_CUBE, &temporary_pivot_material);
	Actor pivot_point = Actor("pivot point");

	using Light::Light;

	RenderCommands getRenderCommands() override;
	void tick(int current_tick) override;
	void loadSettings() override;
	void callToStage(Theatre *parent_theatre) override;
	void takeABow() override;
};

#define RAMIEL_CIRLE    0
#define RAMIEL_APPROACH 1

class Ramiel: public Actor
{
public:
	int movement_type = 0;

	glm::vec3 pivot_position = glm::vec3(0.0f);
	float pivot_radius = 3.0f;
	float pivot_speed = 1.0f;
	float pivot_theta = 0.0f;

	float movement_speed = 1.0f;

	using Actor::Actor;

	void tick(int current_tick) override;
	void loadSettings() override;
};
#endif
