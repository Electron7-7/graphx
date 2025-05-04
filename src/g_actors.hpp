#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
#include "g_devices.hpp"
#include "r_common.hpp"
#include "g_actor.hpp"
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
	float label_alpha = 0.0f;

	using Actor::Actor;

	void tick(const int) override;
	void loadSettings() override;
	RenderCommands getRenderCommands() override;

protected:
	std::shared_ptr<Actor> parent = std::shared_ptr<Actor>(this);
	Sprite label_mesh = Sprite("Untitled Sprite");
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
	// std::shared_ptr<Texture> texture_projection = nullptr;

	using Actor::Actor;

	RenderCommands getRenderCommands() override;
	void loadSettings() override;

protected:
	bool debug_visible = false;
	Material debug_light_mesh_material = Material(LIGHT_DEBUGGING, NO_TEXTURE, 8, 0.0f, light_color * light_energy);
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
	std::shared_ptr<Actor> parent = nullptr;

	using LightSpot::LightSpot;
	LightFlashlight(std::shared_ptr<Actor> = nullptr);

	void setLight(bool is_on);
	void toggleLight(glm::vec3 toggle_color = glm::vec3(0.0f));
	void setLightColor(glm::vec3 color);
	void setLightColor(bool color_toggle);

	void tick(int current_tick) override;
	void loadSettings() override;

private:
	bool start_enabled = true;
	glm::vec3 _color = light_color;
};

class GraphXPlayer: public Actor //public CharacterController(?)
{
public:
	Model player_mesh = Model("Untitled Model");
	Camera player_camera = Camera("Player Camera");
	LightFlashlight* player_flashlight;

	bool do_gravity = true; // Debugging, mostly
	float mouse_sensitivity = 0.05f;
	float movement_speed = 13.0f;
	double lerp_speed = 1.3f;
	float friction = 0.85f;
	float mass = 100.0f;
	float field_of_view = 45.0f; // Make sure to convert to radians when making the perspective matrix 

	// JPH::Ref<JPH::CharacterSettings> player_settings;

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
	// JPH::Ref<JPH::Character> jph_character = nullptr;
	double movement_lerp = 0.0f;
	int last_direction[2] = {0, 0};
};

class LightTesterMover : public Light
{
public:
	glm::vec3 pivot_position = glm::vec3(0.0f);
	float pivot_radius = 3.0f;
	float pivot_speed = 1.0f;
	float pivot_theta = 0.0f;

	Material temporary_pivot_material = Material(true, glm::vec3(1.0f, 0.0f, 0.0f));
	Model temporary_pivot_mesh = Model(temporary_pivot_material);
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
#endif