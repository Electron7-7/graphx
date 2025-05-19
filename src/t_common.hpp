/*#ifndef GRAPHX_THEATRE_COMMON
#define GRAPHX_THEATRE_COMMON
#include <string>
#include <vector>

#ifdef COMPILER_FORWARD_DECLARATIONS // Forward declarations
struct Theatre;
#endif

// Might move these into graphx::interpreter someday...
static constexpr int RAW_DATA           = 1; //< Identifies a vector of strings, using the typedef `gRawData`.
static constexpr int CPP_REFERENCE      = 2; //< Identifies a C++ reference (see `cpp_definitions` in `t_interpreter.cpp`).
static constexpr int EXTERNAL_REFERENCE = 3; //< Identifies an external file's pathname, passed via string.
static constexpr int THEATRE_REFERENCE  = 4; //< Identifies a pointer to a pre-existing Actor/Device in the current Theatre.
static constexpr int SANDWICH           = 5; //< Identifies a unique copy of a pre-existing Actor/Device in the current Theatre.

typedef std::string                              gKey;
typedef std::pair<int, std::string>              gValue;
typedef std::pair<gKey, gValue>                  gStringSetting;
typedef std::vector<std::vector<gStringSetting>> gStringSettings;
typedef std::vector<std::string>                 gRawData;

extern bool loading_new_main_theatre;
extern std::string valid_extensions;

bool        checkForAndLoadExternalTheatres();
void        loadMainTheatre(long theatre_uid);
void        loadChildTheatre(long theatre_uid, Theatre *parent_theatre);
#endif
*/
#ifndef GRAPHX_THEATRE_COMMON
#include <memory>
#include <string>
#include <vector>
#define GRAPHX_THEATRE_COMMON

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
struct Theatre;
#endif

struct StringSetting
{
    // Setting Categories
    static constexpr unsigned int UNDEFINED          = 0; //< Ironically is what helps avoid undefined behaviour.
    static constexpr unsigned int RAW_DATA           = 1; //< Identifies a vector of strings, using the typedef `gRawData`.
    static constexpr unsigned int CPP_REFERENCE      = 2; //< Identifies a C++ reference (see `cpp_definitions` in `t_interpreter.cpp`).
    static constexpr unsigned int EXTERNAL_REFERENCE = 3; //< Identifies an external file's pathname, passed via string.
    static constexpr unsigned int THEATRE_REFERENCE  = 4; //< Identifies a pointer to a pre-existing Actor/Device in the current Theatre.
    static constexpr unsigned int SANDWICH           = 5; //< Identifies a unique copy of a pre-existing Actor/Device in the current Theatre.

    std::string name = "";
    std::string value = "";
    unsigned int category = UNDEFINED;

    StringSetting();
    StringSetting(const std::string& SettingName, const std::string& SettingValue, const unsigned int SettingCategory);

    void changeSetting(const std::string& SettingValue, const int SettingCategory = LEAVE_UNCHANGED);

private:
    static constexpr int LEAVE_UNCHANGED = -1; // Honestly, this is just to make the clangd output/source code more understandable; it's not technically necessary
};

typedef std::vector<StringSetting> StringSettings;

extern std::string valid_extensions; // Todo: this will be replaced by GraphXInterpreter::validExtensions()

void I_LoadNewMainTheatre(long TheatreUID);
void I_LoadChildTheatre(long TheatreUID, std::shared_ptr<Theatre> ParentTheatre);
void I_EmbedExternalTheatre(const std::string& GraphXTheatreFilePath);
bool I_CheckForAndLoadExternalTheatres();
#endif
