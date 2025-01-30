#include "g_common.hpp"
#include "g_actors.hpp"
#include "g_jolt.hpp"
#include "g_math.hpp"
#include <vector>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

GraphXPlayer *current_player = NULL;
glm::vec3 vector3_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 vector3_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 vector3_right = glm::vec3(1.0f, 0.0f, 0.0f);
std::unordered_map<double, Actor *> actor_uid_lookup;

//
// RenderState
//
RenderState::RenderState(glm::vec3 init_position, glm::quat init_quaternion, glm::vec3 init_scale)
: render_position(init_position), render_quaternion(init_quaternion), render_scale(init_scale)
{}

//
// Actor
//
Actor::Actor(std::string new_name, Mesh *init_mesh, glm::vec3 init_position, glm::vec3 init_euler_degrees, glm::vec3 init_scale)
: UID(actor_uid_lookup.size()), mesh(init_mesh), position_global(init_position), scale(init_scale), orientation_front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	actor_type = ACTOR_ACTOR;
	name = new_name;
	actor_uid_lookup.insert(actor_uid_lookup.end(), std::pair<double, Actor *>{UID, this});
	world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);
	quaternion = glm::quat(glm::radians(init_euler_degrees));
	current_state = RenderState(init_position, quaternion, init_scale);
	current_state_copy = current_state;
	previous_state = current_state;
	previous_state_copy = current_state;
	current_state_buffer = { current_state, current_state_copy };
	previous_state_buffer = { previous_state, previous_state_copy };
	updateVectors();
}

void Actor::updateVectors()
{
	orientation_up = quaternion * vector3_up;
	orientation_front = quaternion * vector3_front;
	orientation_right = quaternion * vector3_right;
}

void Actor::updateStates(std::mutex &state_mutex)
{
	std::lock_guard guard(state_mutex);

	// Copy current state into previous state
	previous_state_buffer[state_index] = current_state_buffer[state_index];

	// Update current state
	current_state_buffer[state_index].render_position	=	position_global;
	current_state_buffer[state_index].render_quaternion	=	quaternion;
	current_state_buffer[state_index].render_scale		=	scale;

	// Flip state buffer
	state_index = 1 - state_index;
}

void Actor::tick(int current_tick)
{}

void Actor::callToStage(Theatre *parent_theatre)
{
	std::cout << "\n\t- " << name << std::endl;
}

void Actor::takeABow()
{
	std::cout << "\n\t- (Actor) " << name << std::endl;
}

bool Actor::wantsToBeRendered()
{
	return (wantsToBeBuffered() && visible);
}

bool Actor::wantsToBeBuffered()
{
	if(actor_type == ACTOR_LIGHT)
		return (debug_visible);
	return(mesh != NULL);
}

//
// jolt_collider_options
//
jolt_collider_options::jolt_collider_options(JPH::ShapeSettings *body_shape_settings, JPH::EMotionType body_motion_type, JPH::ObjectLayer body_object_layer, JPH::EActivation body_activation)
: shape_settings(body_shape_settings), motion_type(body_motion_type), object_layer(body_object_layer), activation(body_activation)
{}

//
// PhysicsActor
//
PhysicsActor::PhysicsActor(std::string init_name, std::vector<jolt_collider_options *> init_collider_options, Mesh *init_mesh, glm::vec3 init_position, glm::vec3 init_euler_degrees, glm::vec3 init_scale)
: Actor(init_name, init_mesh, init_position, init_euler_degrees, init_scale), collider_options(init_collider_options)
{
	actor_type = ACTOR_PHYSICS;
}

PhysicsActor::PhysicsActor(std::string init_name, jolt_collider_options *init_collider_options, Mesh *init_mesh, glm::vec3 init_position, glm::vec3 init_euler_degrees, glm::vec3 init_scale)
: Actor(init_name, init_mesh, init_position, init_euler_degrees, init_scale), collider_options(std::vector<jolt_collider_options *>{init_collider_options})
{
	actor_type = ACTOR_PHYSICS;
}

void PhysicsActor::callToStage(Theatre *parent_theatre)
{
	Actor::callToStage(parent_theatre);
	for(jolt_collider_options *collider_data : collider_options)
	{
		collider_data->position = convertMath<JPH::Vec3>(position_global);
		collider_data->quaternion = convertMath<JPH::Quat>(quaternion);
		JPH::BodyID collider_id = J_AddAndCreateBody(collider_data);
		collider_ids.insert(collider_ids.end(), collider_id);
	}
}

void PhysicsActor::takeABow()
{
	for(JPH::BodyID collider_id : collider_ids)
		J_RemoveAndDestroyBody(collider_id);
}

void PhysicsActor::tick(int current_tick)
{}

//
// RigidBodyActor
//
void RigidBodyActor::callToStage(Theatre *parent_theatre)
{
	PhysicsActor::callToStage(parent_theatre);
	reset_position = convertMath<JPH::Vec3>(position_global);
	reset_quaternion = convertMath<JPH::Quat>(quaternion);
}

void RigidBodyActor::tick(int current_tick)
{
	PhysicsActor::tick(current_tick);

	JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
	// body_interface.ActivateBody(collider_ids[controller_collider_index]);
	PRINT(body_interface.IsAdded(collider_ids[controller_collider_index]));
	JPH::Vec3 body_position = body_interface.GetCenterOfMassPosition(collider_ids[controller_collider_index]);
	JPH::Quat body_quaternion = body_interface.GetRotation(collider_ids[controller_collider_index]);

	PRINT(name << "\n\tposition: " << glm::to_string(position_global) << "\n\tquaternion: " << glm::to_string(quaternion));
	PRINT(name << "\n\tbody position: " << body_position << "\n\tbody quaternion: " << body_quaternion);
	position_global = convertMath<glm::vec3>(body_position);
	quaternion = convertMath<glm::quat>(body_quaternion);
	updateVectors();
}

void RigidBodyActor::reset_to_initial_orientation_for_testing()
{
	JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
	body_interface.SetPositionAndRotation(collider_ids[controller_collider_index], reset_position, reset_quaternion, JPH::EActivation::Activate);
	body_interface.SetLinearAndAngularVelocity(collider_ids[controller_collider_index], JPH::Vec3::sZero(), JPH::Vec3::sZero());
}

//
// Camera
//
void Camera::doRotation(glm::vec2 mouse_input)
{
	euler_rotation += euler_rotation_local;
	euler_rotation[0] -= glm::radians(mouse_input[1]);
	euler_rotation[1] -= glm::radians(mouse_input[0]);

	if(std::abs(glm::degrees(euler_rotation[0])) > view_pitch_clamp)
		euler_rotation[0] = glm::radians(view_pitch_clamp * ((glm::degrees(euler_rotation[0]) > 0) - (glm::degrees(euler_rotation[0]) < 0)));
}

void Camera::tick(int current_tick)
{
	position_global = parent->position_global + position_local;
}

//
// GraphXPlayer
//
GraphXPlayer::GraphXPlayer(std::string new_name, glm::vec3 init_position, glm::vec3 init_rotation_euler)
: Actor(new_name, &player_mesh, init_position, init_rotation_euler, glm::vec3(1.5f, 3.0f, 1.5f))
{
	actor_type = ACTOR_PLAYER;
	debug_visible = false;
	player_camera.euler_rotation = glm::radians(init_rotation_euler);
	player_camera.position_global = init_position;
	player_camera.parent = this;
}

void GraphXPlayer::callToStage(Theatre *parent_theatre)
{
	Actor::callToStage(parent_theatre);
	// player_settings = new JPH::CharacterSettings;
	// player_settings->mMaxSlopeAngle = JPH::DegreesToRadians(45.0f);
	// player_settings->mLayer = Layers::MOVING;
	// player_settings->mShape = JPH::RotatedTranslatedShapeSettings(convertMath<JPH::Vec3>(position_global), quaternion, new JPH::CapsuleShape(scale[1], scale[0])).Create().Get();
	// player_settings->mFriction = 10.0f;
	// player_settings->mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -scale[0]);
	// jph_character = new JPH::Character(player_settings, convertMath<JPH::Vec3>(position_global), quaternion, 0, physics_system);
	// jph_character->AddToPhysicsSystem(JPH::EActivation::Activate);
}

void GraphXPlayer::tick(int current_tick)
{
	player_camera.tick(current_tick);
	// JPH::Vec3 jph_position = jph_character->GetPosition();
	// glm::vec3 jph_position_glm = convertMath<glm::vec3>(jph_position);
	// position_global = glm::vec3(jph_position_glm[0], jph_position_glm[1] + scale[1], jph_position_glm[2]);
}

void GraphXPlayer::doMovement(int direction[2])
{
	position_global += orientation_front * static_cast<float>(direction[0] * movement_speed);
	position_global += orientation_right * static_cast<float>(direction[1] * movement_speed);
	// JPH::Vec3 current_velocity = jph_character->GetLinearVelocity();
	// JPH::Vec3 wish_velocity = JPH::Vec3(direction[1] * movement_speed, 0.0f, -direction[0] * movement_speed);
	// JPH::Vec3 new_velocity = current_velocity + wish_velocity;

	// new_velocity.SetX(JPH::Clamp(new_velocity.GetX(), -max_velocity, max_velocity));
	// new_velocity.SetZ(JPH::Clamp(new_velocity.GetZ(), -max_velocity, max_velocity));

	// PRINT("position_global: " << glm::to_string(position_global) << "\ncurrent_velocity: " << current_velocity << "\nwish_velocity: " << wish_velocity << "\nnew_velocity: " << new_velocity);

	// jph_character->SetLinearVelocity(new_velocity);
}

void GraphXPlayer::doMouseMovement(glm::vec2 mouse_offset)
{
	player_camera.doRotation(mouse_offset * mouse_sensitivity);
	quaternion = glm::quat(player_camera.euler_rotation);
	updateVectors();
}

glm::mat4 GraphXPlayer::getViewMatrix()
{
	return glm::lookAt(player_camera.position_global, player_camera.position_global + orientation_front, orientation_up);
}

bool GraphXPlayer::wantsToBeRendered()
{
	return false;
}

//
// Light
//
Light::Light(std::string init_name, float init_intensity, float init_range, float init_falloff, float init_strength, glm::vec3 init_color, glm::vec3 init_position, glm::vec3 init_rotation, glm::vec3 init_scale)
: Actor(init_name, &temporary_light_mesh, init_position, init_rotation, init_scale), light_color(init_color), light_strength(init_strength), range(init_range), intensity(init_intensity), falloff(init_falloff)
{
	actor_type = ACTOR_LIGHT;
	light_type = LIGHT_POINT;
	debug_visible = true;
}

//
// LightDirectional
//
LightDirectional::LightDirectional(std::string init_name, glm::vec3 init_direction, float init_strength, glm::vec3 init_color)
: Light(init_name, 1.0f, 100.0f, 0.0f, init_strength, init_color), direction(init_direction)
{ light_type = LIGHT_DIRECTIONAL; }

//
// LightSpot
//
LightSpot::LightSpot(std::string init_name, float init_intensity, float init_range, float init_falloff, float init_strength, glm::vec3 init_color, float init_inner_cutoff_angle, float init_outer_cutoff_angle, glm::vec3 init_direction, glm::vec3 init_position, glm::vec3 init_rotation)
: Light(init_name, init_intensity, init_range, init_falloff, init_strength, init_color, init_position, init_rotation), direction(init_direction), inner_cutoff_angle(init_inner_cutoff_angle), outer_cutoff_angle(init_outer_cutoff_angle)
{
	light_type = LIGHT_SPOT;
}

glm::vec2 LightSpot::getCutoffAngles()
{
	return glm::vec2
	{
		glm::cos(glm::radians(inner_cutoff_angle)),
		glm::cos(glm::radians(outer_cutoff_angle)),
	};
}

//
// LightFlashlight
//
LightFlashlight::LightFlashlight(std::string init_name, float init_intensity, float init_range, float init_falloff, float init_strength, glm::vec3 init_color, float init_inner_cutoff_angle, float init_outer_cutoff_angle, glm::vec3 init_position_offset, glm::vec3 init_rotation_offset)
: LightSpot(init_name, init_intensity, init_range, init_falloff, init_strength, init_color, init_inner_cutoff_angle, init_outer_cutoff_angle), position_offset(init_position_offset), rotation_offset(init_rotation_offset), _intensity(init_intensity)
{
	light_type = LIGHT_SPOT;
	debug_visible = false;
}

void LightFlashlight::tick(int current_tick)
{
	if(current_player == NULL)
		return;

	if(parent == NULL)
		parent = current_player;

	position_global = parent->position_global + position_offset;
	quaternion = parent->quaternion * glm::quat(glm::radians(rotation_offset));
	direction = quaternion * vector3_front;
}

void LightFlashlight::setLight(bool is_off)
{
	intensity = _intensity + (100.0f * is_off);
}

//
// LightTesterMover
//
LightTesterMover::LightTesterMover(std::string init_name, glm::vec3 init_pivot_position, float init_pivot_radius, float init_pivot_speed, float init_intensity, float init_range, float init_falloff, float init_strength, glm::vec3 init_color)
: Light(init_name, init_intensity, init_range, init_falloff, init_strength, init_color), pivot_position(init_pivot_position), pivot_radius(init_pivot_radius), pivot_speed(init_pivot_speed)
{
	pivot_point.name = "Pivot point Actor for " + name + " LightTesterMover (UID: " + std::to_string(UID) + ")";
	pivot_point.mesh->name = "Pivot Mesh for " + name + " LightTesterMover (UID: " + std::to_string(UID) + ")";
	pivot_point.position_global = init_pivot_position;
}

void LightTesterMover::tick(int current_tick)
{
	pivot_point.position_global = pivot_position;

	position_global[0] = pivot_position[0] + pivot_radius * glm::cos(glm::radians(pivot_theta));
	position_global[1] = pivot_position[1];
	position_global[2] = pivot_position[2] + pivot_radius * glm::sin(glm::radians(pivot_theta));

	pivot_theta += pivot_speed;
	if(pivot_theta >= 360.0f)
		pivot_theta = 0.0f;
}

void LightTesterMover::callToStage(Theatre *parent_theatre)
{
	parent_theatre->actorEnter(&pivot_point);
	pivot_point.callToStage(parent_theatre); // Might be calling callToStage() twice here, will have to test
}