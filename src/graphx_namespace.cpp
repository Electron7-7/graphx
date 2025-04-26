#include "graphx_namespace.hpp"
#include "g_actor.hpp"
#include "g_device.hpp"
#include "g_theatre.hpp"

namespace graphx
{
    namespace debug
    {
        bool actor_debug_menu_open = false;
        float actor_debug_menu_text_scale = 1.8f;
    }

    namespace safety
    {
        Actor actor = Actor("Safety Actor (if you see this, this is the engine trying its very best not to return a nullptr while also not leaking memory!");
        Device device = Device("Safety Device (if you see this, this is the engine trying its very best not to return a nullptr while also not leaking memory!");
    }

    namespace global
    {
        namespace variables
        {
            Theatre current_theatre = Theatre("current_theatre Not Yet Set!");

            glm::vec3 orientation_up(0.0f, 1.0f, 0.0f);
            glm::vec3 orientation_front(0.0f, 0.0f, -1.0f);
            glm::vec3 orientation_right(1.0f, 0.0f, 0.0f);
        }

        namespace state
        {
            bool loading_new_main_theatre = true; // Definitely wanna replace this with something a little more sophisticated.
        }
    }
}