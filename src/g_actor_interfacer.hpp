#ifndef GRAPHX_ACTOR_INTERFACER
#include "g_actor.hpp" // Technically unnecessary
#include "g_theatre.hpp" // Technically unnecessary
#include "graphx_namespace.hpp"
#include "g_actors.hpp"
#include "g_devices.hpp"
#define GRAPHX_ACTOR_INTERFACER

class ActorInterfacer
{
public:
    ActorInterfacer(const Actor*);
    ~ActorInterfacer();

    const bool isType(const graphx::gClass*) const;
    const bool isLight() const;

private:
    const Actor* actor;
    const Actor i_help_avoid_using_nullptr_and_new = Actor("I help avoid undefined behaviour and memory leaks (using nullptr and new)!");
};

#endif