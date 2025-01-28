#include "g_common.hpp"
#include "g_actors.hpp"
#include "g_theatre.hpp"
// #include <Jolt/Physics/Character/Character.h>
// #include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
// #include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

GraphXPlayer *current_player = NULL;

//
// Actor
//
void Actor::updateVectors()
{
	// glm::vec3 new_front;
	// glm::vec3 rotation_euler = glm::eulerAngles(quaternion);

	// new_front[0] = cos(glm::radians(rotation_euler[1])) * cos(glm::radians(rotation_euler[0]));
	// new_front[1] = sin(glm::radians(rotation_euler[0]));
	// new_front[2] = sin(glm::radians(rotation_euler[1])) * cos(glm::radians(rotation_euler[0]));
	
	// orientation_front = glm::normalize(new_front);
	// orientation_right = glm::normalize(glm::cross(orientation_front, world_orientation_up));
	// orientation_up = glm::normalize(glm::cross(orientation_right, orientation_front));

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

void Actor::init(Theatre *parent_theatre)
{
	// quaternion = JPH::Quat::sEulerAngles(convertMath<JPH::Vec3>(rotation_euler));
}

bool Actor::wantsToBeRendered()
{
	if(actor_type == ACTOR_TOOL || actor_type == ACTOR_LIGHT)
		return (debug_visible);

	return ( mesh != NULL && visible );
}

bool Actor::wantsToBeBuffered()
{
	if(actor_type == ACTOR_TOOL || actor_type == ACTOR_LIGHT)
		return (debug_visible);
	return( mesh != NULL );
}

//
// PhysicsActor
//
void PhysicsActor::init(Theatre *parent_theatre)
{
	Actor::init(parent_theatre);
	// collider_settings = JPH::BodyCreationSettings(new JPH::BoxShape(JPH::Vec3(scale[0], scale[1], scale[2])), JPH::RVec3(JPH::Real3(position_global[0], position_global[1], position_global[2])), quaternion, JPH::EMotionType::Dynamic, Layers::MOVING);
	// JPH::MassProperties body_mass_properties;
	// body_mass_properties.ScaleToMass(mass);
	// collider_settings.mMassPropertiesOverride = body_mass_properties;
	// collider_settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
	// reset_position = convertMath<JPH::Vec3>(position_global);
	// reset_quaternion = quaternion;
}

void PhysicsActor::tick(int current_tick)
{
	// JPH::BodyInterface &body_interface = physics_system->GetBodyInterface();
	// position_global = glm::vec3(body_interface.GetCenterOfMassPosition(physics_body_id)[0], body_interface.GetCenterOfMassPosition(physics_body_id)[1], body_interface.GetCenterOfMassPosition(physics_body_id)[2]);
	// quaternion = body_interface.GetRotation(physics_body_id);
}

void PhysicsActor::reset_to_initial_orientation_for_testing()
{
	// JPH::BodyInterface &body_interface = physics_system->GetBodyInterface();
	// body_interface.SetPositionAndRotation(physics_body_id, JPH::RVec3(reset_position), reset_quaternion, JPH::EActivation::Activate);
	// body_interface.SetLinearAndAngularVelocity(physics_body_id, JPH::Vec3::sZero(), JPH::Vec3::sZero());
}

//
// GraphXPlayer
//
void GraphXPlayer::init(Theatre *parent_theatre)
{
	Actor::init(parent_theatre);
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
	// JPH::Vec3 jph_position = jph_character->GetPosition();
	// glm::vec3 jph_position_glm = convertMath<glm::vec3>(jph_position);
	// position_global = glm::vec3(jph_position_glm[0], jph_position_glm[1] + scale[1], jph_position_glm[2]);
}

void GraphXPlayer::doMovement(int direction[2])
{
	// position_global += orientation_front * static_cast<float>(direction[0] * movement_speed);
	// position_global += orientation_right * static_cast<float>(direction[1] * movement_speed);
	// JPH::Vec3 current_velocity = jph_character->GetLinearVelocity();
	// JPH::Vec3 wish_velocity = JPH::Vec3(direction[1] * movement_speed, 0.0f, -direction[0] * movement_speed);
	// JPH::Vec3 new_velocity = current_velocity + wish_velocity;

	// new_velocity.SetX(JPH::Clamp(new_velocity.GetX(), -max_velocity, max_velocity));
	// new_velocity.SetZ(JPH::Clamp(new_velocity.GetZ(), -max_velocity, max_velocity));

	// PRINT("position_global: " << glm::to_string(position_global) << "\ncurrent_velocity: " << current_velocity << "\nwish_velocity: " << wish_velocity << "\nnew_velocity: " << new_velocity);

	// jph_character->SetLinearVelocity(new_velocity);
}

void GraphXPlayer::doMouseMovement(glm::vec2 offset, bool constrain_pitch)
{
	camera_angle += offset * 0.05f;

	if(constrain_pitch && (std::abs(camera_angle[0]) > 89.0f))
		camera_angle[0] = glm::radians(89.0f * ((camera_angle[0] > 0) - (camera_angle[0] < 0)));

	quaternion = glm::angleAxis(glm::radians(camera_angle[1]), orientation_right) + glm::angleAxis(glm::radians(camera_angle[0]), orientation_up);
	updateVectors();
}

glm::mat4 GraphXPlayer::getViewMatrix()
{
	return glm::lookAt(position_global, position_global + orientation_front, orientation_up);
}

bool GraphXPlayer::wantsToBeRendered()
{
	return false;
}

//
// Lights
//
glm::vec2 LightSpot::getCutoffAngles()
{
	return glm::vec2
	{
		glm::cos(glm::radians(inner_cutoff_angle)),
		glm::cos(glm::radians(outer_cutoff_angle)),
	};
}

void LightFlashlight::tick(int current_tick)
{
	if(current_player == NULL)
		return;

	if(parent == NULL)
		parent = current_player;

	position_global = parent->position_global + position_offset;
	quaternion = parent->quaternion + glm::quat(rotation_offset);
	updateVectors();
	direction = orientation_front;
}

void LightFlashlight::setLight(bool is_off)
{
	intensity = _intensity + (100.0f * is_off);
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

void LightTesterMover::init(Theatre *parent_theatre)
{
	parent_theatre->actorEnter(&pivot_point);
	pivot_point.init(parent_theatre); // Might be calling init() twice here, will have to test
}