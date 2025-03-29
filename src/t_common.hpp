#ifndef GRAPHX_THEATRE_FILE_FORMAT
#define GRAPHX_THEATRE_FILE_FORMAT
#include "g_common_fwd.hpp"
#include <string>
#include <vector>

// Might move these into graphx::interpreter someday...
static constexpr int RAW_DATA           = 1; //< Identifies a vector of strings, using the typedef `gRawData`.
static constexpr int CPP_REFERENCE      = 2; //< Identifies a C++ reference (see `cpp_definitions` in `t_interpreter.cpp`).
static constexpr int EXTERNAL_REFERENCE = 3; //< Identifies an external file's pathname, passed via string.
static constexpr int THEATRE_REFERENCE  = 4; //< Identifies a pointer to a pre-existing Actor/Device in the current Theatre.
static constexpr int SANDWICH           = 5; //< Identifies a unique copy of a pre-existing Actor/Device in the current Theatre.

namespace graphx
{
	namespace interpreter
	{
		typedef std::string                              gKey;
		typedef std::pair<int, std::string>              gValue;
		typedef std::pair<gKey, gValue>                  gStringSetting;
		typedef std::vector<std::vector<gStringSetting>> gStringSettings;
		typedef std::vector<std::string>                 gRawData;
	}
}

extern bool loading_new_main_theatre;
extern std::string valid_extensions;

bool        checkForAndLoadExternalTheatres();
void        loadMainTheatre(long theatre_uid);
void        loadChildTheatre(long theatre_uid, Theatre *parent_theatre);
#endif
