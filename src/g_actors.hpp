#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
#include "g_actor.hpp"
#include "g_devices.hpp"
#include "r_common.hpp"
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

class Label : public Actor
{
public:
	float label_alpha = 0.0f;

	using Actor::Actor;

	void tick(const int) override;
	void loadSettings() override;
	RenderCommands getRenderCommands() override;

protected:
	Actor* parent = this;
	Sprite label_mesh = Sprite();
	TextRenderCmd text_render_command;
};

class Camera : public Actor
{
public:
	float view_pitch_clamp_degrees = 89.0f;

	using Actor::Actor;

	void loadSettings() override;
	void processMouse(GLFWwindow* window, double x_position_in, double y_position_in) override;

private:
	glm::vec2 mouse_last = glm::vec2(0.0f);
};

class GraphXPlayer: public Actor //public CharacterController(?)
{
public:
	Model player_mesh = Model();
	Camera player_camera;
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
	void tick(const int) override;
	void loadSettings() override;

private:
	glm::vec3 flashlight_debug_toggle_color_god_damn_this_variable_name_is_long = glm::vec3(1.0f, 0.0f, 0.0f);
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

	const graphx::gClass* getLightType() const;
	const bool isLightType(const graphx::gClass* light_type) const;
	const bool isLightType(const graphx::gClass& light_type) const;

	RenderCommands getRenderCommands() override;
	void loadSettings() override;

protected:
	const graphx::gClass* my_light_type = nullptr;
	bool debug_visible = false;
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

	Material temporary_pivot_material = Material(true, glm::vec3(1.0f, 0.0f, 0.0f));
	Model temporary_pivot_mesh = Model();
	Actor pivot_point = Actor("pivot point");

	using Light::Light;

	void tick(const int) override;
	void loadSettings() override;
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

	void tick(const int) override;
	void loadSettings() override;
};

extern glm::vec3 vector3_up;
extern glm::vec3 vector3_front;
extern glm::vec3 vector3_right;

template<typename T> Actor* createNewActor(const graphx::gID& new_uid, const graphx::gSettings& new_settings = graphx::gSettings()) { return new T(new_uid, new_settings); }
#endif
