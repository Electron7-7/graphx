// l_state.hpp - Code relating to Actor/generic objects' states
#include "sanity.hpp"

/*
State
-
i know that i'm going to need to compare states to either check if it's changed or to confirm i'm using the same state (mainly for double buffering).

the problem is how i'm going to do that. one idea was to implement a sort of "hash" creator function that would generate a unique number based on
all of the state's variables. however, this won't work if literally any of my variables are floats (which many will be), since in order to not cause
the hash generation to be gargantuan, I'd have to know a lot more about hash generation and I don't wanna spend all that time doing something barely
useful in this situation.

instead, i'm going to implement a simple check, like a flag that gets set when the state is created, for code that doesn't care too much about the
inner details of that state (like exact location, rotation, etc), OR include an inner state machine that can be customized, like for enemy and player
state regarding higher-level game mechanics (like, "crouching", "standing", "shooting", "chasing player", etc).

but that won't be all; i'll also implement a way to check on certain details for code that really needs to (i.e: the gametick interpolation logic).
this could just boil down to testing the inner variable "global_location" and I don't have to write any custom checker function. that sounds nice,
but it also sounds too easy, so I may be misinterpreting how this kind of "state" is supposed to be used.

I could also use pointers? like, can you make sure that an object's state hasn't changed by comparing the pointer to that state with the previous 
value of the state's pointer? I'll have to check.

this also means that I'm going to need to know for sure whether or not I want to make this "state" struct mutable or immutable.

I could also try learning/using Templates.

I could also make an abstract "State" struct, then make inherited structs for more specific cases (i.e: ActorState, RenderState, etc.).
If I do this, I'd need to make sure the abstract "State" struct is where I keep all the generic functions, variables, and constructors; which means that
the abstract "State" struct is where I'll need to write any generic state checking functions and also account for any number of variables. This is where
Templates could come in handy.
*/

#ifndef GRAPHX_STATE_OBJECT_ORIENTATED
#define GRAPHX_STATE_OBJECT_ORIENTATED
//
// State
//
struct GenericState
{

};

//
// Render State
//
struct RenderState
{
	glm::vec3 position_global;
	// glm::vec4 rotation_quaternion;
	glm::vec3 rotation_euler;

	// auto tie() const { return std::tie(); }
	// bool operator==(const RenderState& right_state) const { return tie() == right_state.tie(); }
};
#endif


#ifndef GRAPHX_STATE_FUNCTIONAL
#define GRAPHX_STATE_FUNCTIONAL
#endif