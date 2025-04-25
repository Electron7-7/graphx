#include "graphx_namespace.hpp"
#include "graphx_classes.hpp"
#include "g_actor.hpp"
#include "g_device.hpp"

//--------------------------------------
// Definitions for: graphx_namespace.hpp
//--------------------------------------
namespace graphx
{
    namespace safety
    {
        Actor actor(graphx::classes::ACTOR, graphx::gID(SAFETY_ID, "Safety Actor (the default constructor was called, or I'm avoiding nullptr)"));
        Device device(graphx::classes::DEVICE, graphx::gID(SAFETY_ID, "Safety Device (the default constructor was called, or I'm avoiding nullptr)"));
    }

    namespace debug
    {
        bool actor_debug_menu_open = false;
        float actor_debug_menu_text_scale = 1.8f;
    }

    namespace global
    {
        glm::vec3 orientation_up(0.0f, 1.0f, 0.0f);
        glm::vec3 orientation_front(0.0f, 0.0f, -1.0f);
        glm::vec3 orientation_right(1.0f, 0.0f, 0.0f);
    }

    //-----
    // gID
    //-----
    gID::gID(const int new_id, const std::string& new_name)
    : uid(new_id), name(new_name)
    {}

    gID::gID(const gID& copy_me)
    : gID(copy_me.uid, copy_me.name)
    {}

    gID::gID(const std::string& new_name)
    : gID(-1, new_name)
    {}

    gID::gID(const char* new_name)
    : gID(-1, new_name)
    {}

    gID::gID(const int new_id)
    : gID(new_id, "A Class With No Name")
    {}

    gID::gID()
    : gID("A Class With No Name")
    {}

    const gID gID::EMPTY = gID();

    // Comparing gID to gID
    const bool gID::operator==(const gID& compare_against) const { return (uid == compare_against.uid);  }
    const bool gID::operator!=(const gID& compare_against) const { return !(*this == compare_against); }
    const bool gID::operator< (const gID& compare_against) const { return (uid < compare_against.uid);   }
    const bool gID::operator> (const gID& compare_against) const { return (uid > compare_against.uid);   }
    const bool gID::operator<=(const gID& compare_against) const { return !(*this > compare_against);  }
    const bool gID::operator>=(const gID& compare_against) const { return !(*this < compare_against);  }

    const std::string gID::toString() const
    { return std::string("{" + std::to_string(uid) + ", " + name + "}"); }

    //-------
    // gClass
    //-------
    gClass::gClass(const char init_name[CLASS_NAME_MAX_SIZE_BYTES], const int init_id, Actor*(*init_new_actor)(const graphx::gID&, const graphx::gSettings&), Device*(*init_new_device)(const graphx::gID&, const graphx::gSettings&), const glm::vec3& init_debugging_color)
    : id(init_id), name(init_name), new_actor(init_new_actor), new_device(init_new_device), debugging_color(init_debugging_color)
    {}

    gClass::gClass(const char init_name[CLASS_NAME_MAX_SIZE_BYTES], const int init_id, Actor*(*new_actor_function)(const graphx::gID&, const graphx::gSettings&), const glm::vec3& init_debugging_color)
    : gClass(init_name, init_id, new_actor_function, nullptr, init_debugging_color)
    {}

    gClass::gClass(const char init_name[CLASS_NAME_MAX_SIZE_BYTES], const int init_id, Device*(*new_device_function)(const graphx::gID&, const graphx::gSettings&), const glm::vec3& init_debugging_color)
    : gClass(init_name, init_id, nullptr, new_device_function, init_debugging_color)
    {}

    gClass::gClass()
    : gClass("INVALID_TYPE", 0, nullptr, nullptr, glm::vec3(1.0f, 0.0f, 0.0f))
    {}

    gClass::gClass(const std::string& name)
    : gClass(lookupByName(name))
    {}

    gClass::gClass(const char* lookup_by_name)
    : gClass(lookupByName(lookup_by_name))
    {}

    gClass::gClass(const int& id)
    : gClass(lookupById(id))
    {}

    const graphx::gClass& graphx::gClass::lookupByName(const std::string& to_copy)
    {
        for(auto& valid_class : classes::valid_classes)
            if(!to_copy.compare(valid_class->name))
                return *valid_class;
        return graphx::gClass::INVALID_TYPE;
    }

    const graphx::gClass& graphx::gClass::lookupById(const int& to_copy)
    {
        for(auto& valid_class : classes::valid_classes)
            if(valid_class->id == to_copy)
                return *valid_class;
        return graphx::gClass::INVALID_TYPE;
    }

    // Comparing gClass to gClass
    const bool gClass::operator==(const gClass& compare_against) const { return (id == compare_against.id);  }
    const bool gClass::operator!=(const gClass& compare_against) const { return !(*this == compare_against); }
    const bool gClass::operator< (const gClass& compare_against) const { return (id < compare_against.id);   }
    const bool gClass::operator> (const gClass& compare_against) const { return (id > compare_against.id);   }
    const bool gClass::operator<=(const gClass& compare_against) const { return !(*this > compare_against);  }
    const bool gClass::operator>=(const gClass& compare_against) const { return !(*this < compare_against);  }
    // Comparing gClass to gClass* (left and right sided)
    const bool operator==(const gClass& object,  const gClass* pointer) { return (pointer && object == *pointer); }
    const bool operator!=(const gClass& object,  const gClass* pointer) { return (pointer && object != *pointer); }
    const bool operator==(const gClass* pointer, const gClass& object)  { return (object == *pointer);            }
    const bool operator!=(const gClass* pointer, const gClass& object)  { return (object != *pointer);            }
    // Comparing gClass to int
    const bool gClass::operator==(const int&    compare_against) const { return (id == compare_against);     }
    const bool gClass::operator!=(const int&    compare_against) const { return !(*this == compare_against); }
    const bool gClass::operator< (const int&    compare_against) const { return (id < compare_against);      }
    const bool gClass::operator> (const int&    compare_against) const { return (id > compare_against);      }
    const bool gClass::operator<=(const int&    compare_against) const { return !(*this > compare_against);  }
    const bool gClass::operator>=(const int&    compare_against) const { return !(*this < compare_against);  }
    // Comparing gClass to std::string
    const bool gClass::operator==(const std::string& compare_against) const { return (name == compare_against); }
    const bool gClass::operator!=(const std::string& compare_against) const { return !(*this == compare_against); }
}