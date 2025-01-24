#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
#include "sanity.hpp"
#include "r_common.hpp"
#include "g_theatre.hpp"
#include "g_common.hpp"
#include <vector>
#include <mutex>

#define EULER_CHANGE_QUATERNION 0
#define QUATERNION_CHANGE_EULER 1

#define ACTOR_ACTOR 			0
#define ACTOR_TOOL  			1
#define ACTOR_LIGHT 			1

#define LIGHT_POINT				0
#define LIGHT_DIRECTIONAL		1
#define LIGHT_SPOT				2

struct RenderState
{
	glm::vec3 render_position;
	glm::quat render_quaternion;
	// glm::vec3 render_euler;
	glm::vec3 render_scale;

	RenderState(glm::vec3 init_position = glm::vec3(0.0f), glm::quat init_quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3 init_scale = glm::vec3(1.0f))
	: render_position(init_position), render_quaternion(init_quaternion), render_scale(init_scale)
	{}
};

class Actor
{
public:
	Mesh mesh;
	// std::vector<Device *> devices;
	Collider *collider = NULL; // Replace with devices vector after testing
	Collider test_collider;

	RenderState current_state;
	RenderState current_state_copy = current_state;

	RenderState previous_state = current_state;
	RenderState previous_state_copy = current_state;

	std::vector<RenderState> current_state_buffer	=	{ current_state,	current_state_copy	};
	std::vector<RenderState> previous_state_buffer	=	{ previous_state,	previous_state_copy	};

	int state_index = 0;

	unsigned int actor_type;
	bool visible = true;
	std::string name;
	float movement_speed = 1.0f;
	float mass = 1.0f; // in kg

	glm::vec3 position_global;
	glm::quat rotation_quaternion;
	glm::vec3 rotation_euler;
	glm::vec3 scale = glm::vec3(1.0f);
	glm::vec2 velocity_horizontal;
	glm::vec3 velocity;

	glm::vec3 orientation_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;

	glm::vec3 world_orientation_up;

	Actor(std::string new_name, Mesh init_mesh = Mesh(), glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_rotation_euler = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(1.0f))
	: mesh(init_mesh), test_collider(Collider(init_scale, init_position)), position_global(init_position), rotation_euler(init_rotation_euler), scale(init_scale), orientation_front(glm::vec3(0.0f, 0.0f, -1.0f))
	{
		actor_type = ACTOR_ACTOR;
		name = new_name;
		world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);
		rotation_euler = init_rotation_euler;
		rotation_quaternion = glm::quat(init_rotation_euler);
		current_state = RenderState(init_position, rotation_quaternion);
		updateVectors();
		collider = &test_collider;
	}

	virtual void Tick(int current_tick);
	virtual void init(Theatre *parent_theatre);
	virtual void updateStates(std::mutex &state_mutex);
	virtual bool wantsToBeRendered();
	virtual bool wantsToBeBuffered();

protected:
	bool debug_visible;

	void updateRotation(bool override_which);
	void updateVectors();
};

class GraphXPlayer: public Actor
{
public:
	float mouse_sensitivity;
	float movement_speed = 0.05f;

	GraphXPlayer(std::string new_name, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_rotation_euler = glm::vec3(0.0f, -90.0f, 0.0f))
	: Actor(new_name, Mesh(this), init_position, init_rotation_euler), mouse_sensitivity(INIT_SENSITIVITY)
	{}

	glm::mat4 getViewMatrix();
	void doMouseMovement(std::vector<float> offset, bool constrain_pitch = true);
	void doMovement(int direction[2]);

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

class ControlledTester: public Actor
{
public:
	float movement_speed = 0.025f;
	int movement_direction[3];

	ControlledTester(std::string init_name, Mesh init_mesh, glm::vec3 init_position = glm::vec3(0.0f, 3.0f, -3.0f), glm::vec3 init_scale = glm::vec3(1.0f))
	: Actor(init_name, init_mesh, init_position, glm::vec3(0.0f, 0.0f, 0.0f), init_scale)
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
	: Actor(init_name, Mesh(this, Material(TOOL_TEXTURE_LIGHT, TOOL_TEXTURE_LIGHT, 0, 0.0f)), init_position, init_rotation, init_scale), light_color(init_color), light_strength(init_strength), range(init_range), intensity(init_intensity), falloff(init_falloff)
	{
		actor_type = ACTOR_TOOL;
		light_type = LIGHT_POINT;
		debug_visible = true;
		collider = NULL;
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
	: Light(init_name, init_intensity, init_range, init_falloff, init_strength, init_color), pivot_position(init_pivot_position), pivot_radius(init_pivot_radius), pivot_speed(init_pivot_speed), pivot_point(Actor(std::string("pivot point for ") + init_name, Mesh(&pivot_point, Material(true, glm::vec3(1.0f, 0.0f, 0.0f))), init_pivot_position, glm::vec3(0.0f), glm::vec3(0.2f)))
	{
		pivot_point.mesh.name = "PIVOT";
	}

	void Tick(int current_tick) override;
	void init(Theatre *parent_theatre) override;
};

extern GraphXPlayer *current_player;
#endif