#ifndef GRAPHX_THEATRE_FILE_FORMAT
#define GRAPHX_THEATRE_FILE_FORMAT
#include <string>
#include <filesystem> // Yes, the devil hath been invoked... I'm sorry

struct Theatre; // Forward Declaration

// Might move these into graphx::interpreter someday...
static constexpr int RAW_DATA           = 1; //< Identifies a vector of strings, using the typedef `gRawData`.
static constexpr int CPP_REFERENCE      = 2; //< Identifies a C++ reference (see `cpp_definitions` in `t_interpreter.cpp`).
static constexpr int EXTERNAL_REFERENCE = 3; //< Identifies an external file's pathname, passed via string.
static constexpr int THEATRE_REFERENCE  = 4; //< Identifies a pointer to a pre-existing Actor/Device in the current Theatre.
static constexpr int SANDWICH           = 5; //< Identifies a unique copy of a pre-existing Actor/Device in the current Theatre.

extern std::string valid_extensions;

bool checkForAndLoadExternalTheatres();
void loadMainTheatre(long theatre_uid);
void loadChildTheatre(long theatre_uid, Theatre* parent_theatre);
void embedExternalTheatre(std::filesystem::path theatre_file_path);
#endif
