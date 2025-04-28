#ifndef GRAPHX_THEATRE_FILE_FORMAT
#define GRAPHX_THEATRE_FILE_FORMAT
#include <string>
#include <vector>
#include <filesystem> // Yes, the devil hath been invoked... I'm sorry

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
struct Theatre;
#endif

typedef std::string                              gKey;
typedef std::pair<int, std::string>              gValue;
typedef std::pair<gKey, gValue>                  gStringSetting;
typedef std::vector<std::vector<gStringSetting>> gStringSettings;
typedef std::vector<std::string>                 gRawData;

// Might move these into graphx::interpreter someday...
static constexpr int RAW_DATA           = 1; //< Identifies a vector of strings, using the typedef `gRawData`.
static constexpr int CPP_REFERENCE      = 2; //< Identifies a C++ reference (see `cpp_definitions` in `t_interpreter.cpp`).
static constexpr int EXTERNAL_REFERENCE = 3; //< Identifies an external file's pathname, passed via string.
static constexpr int THEATRE_REFERENCE  = 4; //< Identifies a pointer to a pre-existing Actor/Device in the current Theatre.
static constexpr int SANDWICH           = 5; //< Identifies a unique copy of a pre-existing Actor/Device in the current Theatre.

void I_LoadNewMainTheatre(long TheatreUID);
void I_LoadChildTheatre(long TheatreUID, Theatre* ParentTheatre);
void I_EmbedExternalTheatre(std::filesystem::path GraphXTheatreFilePath);
bool I_CheckForAndLoadExternalTheatres();
#endif
