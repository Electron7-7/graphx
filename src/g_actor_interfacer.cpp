#include "g_actor_interfacer.hpp"

using namespace graphx;

//
// ActorInterfacer
//
ActorInterfacer::ActorInterfacer(const Actor* new_actor)
{
    if(new_actor != nullptr)
    {
        delete actor;
        actor = new_actor;
    }
}

ActorInterfacer::~ActorInterfacer()
{
    delete actor;
}