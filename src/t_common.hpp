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

void I_LoadNewMainTheatre(long TheatreUID);
void I_LoadChildTheatre(long TheatreUID, std::shared_ptr<Theatre> ParentTheatre);
void I_EmbedExternalTheatre(const std::string& GraphXTheatreFilePath);
bool I_CheckForAndLoadExternalTheatres();
#endif
