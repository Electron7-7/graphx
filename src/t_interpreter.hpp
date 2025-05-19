#ifndef GRAPHX_THEATRE_INTERPRETER
#include "t_common.hpp"
#define GRAPHX_THEATRE_INTERPRETER

// DELETE THESE WHEN I CHANGE gSettings TO BE A STRUCT
#include <any>
#include <unordered_map>
#ifdef COMPILER_FORWARD_DECLARATIONS
// struct gSettings;
typedef std::pair<int, std::any> gSetting;
typedef std::unordered_map<std::string, gSetting> gSettings;
#endif

class GraphXTheatreInterpreter
{
public:
    // This is how I keep track of supported file types/extensions without having to write them out more than once.
    // I define specific file types as strings that contain all the supported file extensions and I
    // add all these strings to "valid_extensions", which is what "loadExternalFile" uses to check if a
    // setting is referencing a supported file type.
    std::string graphx_theatre_extensions = "gt";
    std::string three_dee_model_extensions = "obj";
    std::string image_extensions = "png jpg jpeg bmp webp";

    std::string validExtensions() const;

    std::vector<StringSettings> theatreParser(std::string TheatreData);
    std::string getCategoryString(int Category);
    std::string getTheatreStructure(const std::vector<StringSettings>& AllSettings);

    void loadTheatre(const long TheatreID, Theatre& OutputTheatre);
    void interpretRawData(gSettings& Settings, const std::string& Name, const std::string& RawData);
    void interpretCppReference(gSettings& Settings, const std::string& Name, const std::string& Reference);
    void interpretExternalReference(gSettings& Settings, const std::string& Name, const std::string& Reference);
    void interpretTheatreReference(gSettings& Settings, const std::string& Name, const std::string& ReferencedName, Theatre& CurrentTheatre, std::vector<StringSettings>& AllSettings);
    void interpretSandwich(gSettings& Settings, std::vector<StringSettings>& AllSettings, const std::string& SandwichName, int& FirstIterator, int& SecondIterator, const unsigned long& NumberOfSettings, Theatre& CurrentTheatre);
private:
    // This is just me making the error printout easier to find and add to
    std::string what_are_the_valid_extensions =              \
        "(GraphXTheatre)\n\t" + graphx_theatre_extensions  + \
        "(3D Model)\n\t"      + three_dee_model_extensions + \
        "(Image)\n\t"         + image_extensions;
};
#endif