#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
#include "g_common.hpp"
#include "g_jolt.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Character/Character.h>

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
	void youGotACallBack(graphx::gSettings new_settings = empty_settings) override;
	void callToStage(Theatre *parent_theatre) override;
	void takeABow() override;

	virtual bool isPhysicsActor() override final;
	virtual void reset_to_initial_orientation_for_testing();

protected:
	JPH::Vec3 reset_position;
	JPH::Quat reset_quaternion;
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
	RigidBodyActor();

	void tick(int current_tick) override;
	void youGotACallBack(graphx::gSettings new_settings = empty_settings) override;
	void callToStage(Theatre *parent_theatre) override;
	void takeABow() override;

	void reset_to_initial_orientation_for_testing() override;
};

class StaticBodyActor : public PhysicsActor
{
public:
	StaticBodyActor();

	void youGotACallBack(graphx::gSettings new_settings = empty_settings) override;
	void callToStage(Theatre *parent_theatre) override;
	void takeABow() override;
};

class Camera : public Actor
{
public:
	float view_pitch_clamp = 89.0f;
	glm::vec3 euler_rotation = glm::radians(glm::vec3(0.0f, -90.0f, 0.0f));
	glm::vec3 euler_rotation_local = glm::vec3(0.0f);

	Camera();

	void tick(int current_tick) override;
	void youGotACallBack(graphx::gSettings new_settings = empty_settings) override;
	void doRotation(glm::vec2 mouse_input);

protected:
	glm::vec3 position_global = glm::vec3(0.0f);
	glm::vec3 position_local = glm::vec3(0.0f, 3.0f, 0.0f); // temporary default offset
};

class GraphXPlayer: public Actor //public CharacterController(?)
{
public:
	Mesh player_mesh = Mesh();
	Camera player_camera;
	LightFlashlight *player_flashlight = nullptr;

	float mouse_sensitivity = 0.05f;
	float movement_speed = 13.0f;
	double lerp_speed = 1.3f;
	float friction = 0.85f;
	float mass = 100.0f;
	float field_of_view = 45.0f; // Make sure to convert to radians when making the perspective matrix 

	JPH::Ref<JPH::CharacterSettings> player_settings;

	GraphXPlayer(std::string new_name = "Untitled GraphXPlayer", glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_rotation_euler = glm::vec3(0.0f));

	glm::mat4 getViewMatrix();
	glm::vec3 getViewPosition();
	void processMouse(GLFWwindow *window, double x_position_in, double y_position_in) override;
	void processInput(GLFWwindow *window) override;
	void processKey(GLFWwindow *window, int key, int scancode, int action, int mods) override;
	void doMouseMovement(glm::vec2 mouse_offset);
	void doMovement(int direction[2]);
	bool wantsToBeRendered() override;
	void tick(int current_tick) override;
	void youGotACallBack(graphx::gSettings new_settings = empty_settings) override;
	void callToStage(Theatre *parent_theatre) override;
	void takeABow() override;

private:
	glm::vec2 mouse_last = glm::vec2(0.0f);
	bool flashlight_toggle = true;
	bool flashlight_color_toggle = false;
	JPH::Ref<JPH::Character> jph_character = nullptr;
	double movement_lerp = 0.0f;
	int last_direction[2] = {0, 0};
};

class Light : public Actor
{
public:
	bool debug_visible = false;

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

	Light(std::string init_name = "UNTITLED_LIGHT");

	bool isLightType(graphx::gClass light_type);
	graphx::gClass const &getLightType();

	virtual LightData getLightData();

	void youGotACallBack(graphx::gSettings new_settings = empty_settings) override;

protected:
	graphx::gClass my_light_type;
};

class LightDirectional : public Light
{
public:
	glm::vec3 directional_direction = glm::vec3(0.0f, -0.7f, 0.2f); // Funny name

	LightDirectional(std::string init_name = "UNTITLED_DIRECTIONAL_LIGHT");

	LightData getLightData() override;

	void youGotACallBack(graphx::gSettings new_settings = empty_settings) override;
};

class LightSpot : public Light
{
public:
	glm::vec3 spot_direction = glm::vec3(0.1f, -0.5f, 0.8f);
	float spot_angle = 17.5f;
	float spot_angle_fade = 5.0f;

	LightSpot(std::string init_name = "UNTITLED_SPOT_LIGHT");

	LightData getLightData() override;

	void youGotACallBack(graphx::gSettings new_settings = empty_settings) override;
};

class LightFlashlight: public LightSpot
{
public:
	GraphXPlayer *parent = nullptr;
	glm::vec3 position_offset = glm::vec3(0.0f);
	glm::vec3 rotation_offset = glm::vec3(0.0f);

	LightFlashlight(std::string init_name = "UNTITLED_FLASHLIGHT");

	void setLight(bool is_on);
	void setLightColor(glm::vec3 color);
	void setLightColor(bool color_toggle);

	void tick(int current_tick) override;
	void youGotACallBack(graphx::gSettings new_settings = empty_settings) override;

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

	Material temporary_pivot_material = Material(true, glm::vec3(1.0f, 0.0f, 0.0f));
	Mesh temporary_pivot_mesh = Mesh(&temporary_pivot_material);
	Actor pivot_point = Actor("pivot point", &temporary_pivot_mesh, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.2f));

	LightTesterMover(std::string init_name = "UNTITLED_LIGHT_TESTER_MOVER");

	void tick(int current_tick) override;
	void youGotACallBack(graphx::gSettings new_settings = empty_settings) override;
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

	Ramiel();

	void tick(int current_tick) override;
	void youGotACallBack(graphx::gSettings new_settings = empty_settings) override;
};

extern glm::vec3 vector3_up;
extern glm::vec3 vector3_front;
extern glm::vec3 vector3_right;

template<typename T> Actor *createNewActor() { return new T; }
#endif
