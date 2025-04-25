#include "graphx_namespace.hpp"
using namespace graphx;

bool debug::actor_debug_menu_open = false;
float debug::actor_debug_menu_text_scale = 1.8f;
const gUID gUID::INVALID_UID = gUID(); // INVALID_UID is the default constructor

//-----
// gUID
//-----
gUID::gUID(const int new_id, const std::string& new_name)
: id(new_id), name(new_name)
{}

gUID::gUID(const gUID& copy_me)
: gUID(copy_me.id, copy_me.name)
{}

gUID::gUID()
: gUID(-1, "A Class With No Name")
{}

// Comparing gUID to gUID
const bool gUID::operator==(const gUID& compare_against) const { return (id == compare_against.id);  }
const bool gUID::operator!=(const gUID& compare_against) const { return !(*this == compare_against); }
const bool gUID::operator< (const gUID& compare_against) const { return (id < compare_against.id);   }
const bool gUID::operator> (const gUID& compare_against) const { return (id > compare_against.id);   }
const bool gUID::operator<=(const gUID& compare_against) const { return !(*this > compare_against);  }
const bool gUID::operator>=(const gUID& compare_against) const { return !(*this < compare_against);  }
// Comparing gUID to int
const bool gUID::operator==(const int&  compare_against) const { return (id == compare_against);     }
const bool gUID::operator!=(const int&  compare_against) const { return !(*this == compare_against); }
const bool gUID::operator< (const int&  compare_against) const { return (id < compare_against);      }
const bool gUID::operator> (const int&  compare_against) const { return (id > compare_against);      }
const bool gUID::operator<=(const int&  compare_against) const { return !(*this > compare_against);  }
const bool gUID::operator>=(const int&  compare_against) const { return !(*this < compare_against);  }
// Comparing gUID to std::string
const bool gUID::operator==(const std::string& compare_against) const { return (name == compare_against);   }
const bool gUID::operator!=(const std::string& compare_against) const { return !(*this == compare_against); }

const std::string gUID::toString() const
{
    return std::string("{" + std::to_string(id) + ", " + name + "}");
}

//-------
// gClass
//-------
// The constructors are all constexpr, so they have to be defined in the header file
//-------
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

// Conversions
constexpr gClass::operator int() const { return id; }
constexpr gClass::operator std::string() const { return std::string(name); }