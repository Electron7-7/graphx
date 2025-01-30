#include "g_jolt.hpp"
#include <iostream>

JPH_SUPPRESS_WARNINGS

bool GraphXObjectLayerPairFilter::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const
{
	switch(inObject1)
	{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
	}
}

//
// GraphXBroadPhaseLayerInterface
//
GraphXBroadPhaseLayerInterface::GraphXBroadPhaseLayerInterface()
{
	mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
	mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
}

JPH::uint GraphXBroadPhaseLayerInterface::GetNumBroadPhaseLayers() const
{
	return BroadPhaseLayers::NUM_LAYERS;
}

JPH::BroadPhaseLayer GraphXBroadPhaseLayerInterface::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const
{
	JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
	return mObjectToBroadPhase[inLayer];
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char *GraphXBroadPhaseLayerInterface::GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const
{
	switch ((JPH::BroadPhaseLayer::Type)inLayer)
	{
		case(JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
			return "NON_MOVING";
		case(JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
			return "MOVING";
		default:
			JPH_ASSERT(false);
			return "INVALID";
	}
}
#endif

//
// GraphXObjectVsBroadPhaseLayerFilter
//
bool GraphXObjectVsBroadPhaseLayerFilter::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
{
	switch(inLayer1)
	{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
	}
}

//
// GraphXContantListener
//
JPH::ValidateResult GraphXContactListener::OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult)
{
	// std::cout << "Contact validate callback" << std::endl;
	return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void GraphXContactListener::OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings)
{
	// std::cout << "A contact was added" << std::endl;
}

void GraphXContactListener::OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings)
{
	// std::cout << "A contact was persisted" << std::endl;
}

void GraphXContactListener::OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair)
{
	// std::cout << "A contact was removed" << std::endl;
}

//
// GraphXBodyActivationListener
//
void GraphXBodyActivationListener::OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData)
{
	// std::cout << "A body got activated" << std::endl;
}

void GraphXBodyActivationListener::OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData)
{
	// std::cout << "A body went to sleep" << std::endl;
}