#ifndef GRAPHX_JOLT
#define GRAPHX_JOLT
#include <iostream>
#include <cstdarg>
#include <thread>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

//
// Boiler-Plate & Setup Code (straight out of the Jolt HelloWorld.cpp example, lmao)
//
namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr JPH::uint NUM_LAYERS(2);
};

class GraphXObjectLayerPairFilter : public JPH::ObjectLayerPairFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override;
};

class GraphXBroadPhaseLayerInterface final : public JPH::BroadPhaseLayerInterface
{
public:
	GraphXBroadPhaseLayerInterface();

	virtual JPH::uint GetNumBroadPhaseLayers() const override;
	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char *GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;
#endif

private:
	JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

class GraphXObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override;
};

class GraphXContactListener : public JPH::ContactListener
{
	virtual JPH::ValidateResult OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override;
	virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;
	virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;
	virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override;
};

class GraphXBodyActivationListener : public JPH::BodyActivationListener
{
public:
	virtual void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override;
	virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override;
};
//
// and now, original code...
//
struct jolt_collider_options
{
	JPH::Vec3			position;
	JPH::Quat			quaternion;
	JPH::ShapeSettings *shape_settings;
	JPH::EMotionType	motion_type;
	JPH::ObjectLayer	object_layer;
	JPH::EActivation	activation;

	jolt_collider_options(JPH::ShapeSettings *body_shape_settings, JPH::EMotionType body_motion_type, JPH::ObjectLayer body_object_layer, JPH::EActivation body_activation);
};

extern JPH::PhysicsSystem 		jolt_physics_system;
extern JPH::TempAllocatorImpl 	jolt_temp_allocator;
extern JPH::JobSystemThreadPool jolt_job_system;

void J_InitJolt();
JPH::BodyID J_AddAndCreateBody(jolt_collider_options *collider_options);
void J_StopJolt();
void J_RemoveAndDestroyBody(JPH::BodyID body_id);
#endif