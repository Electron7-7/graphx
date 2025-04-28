#ifndef GRAPHX_THEATRE_INTERPRETER
#include "t_common.hpp"
#include "graphx_namespace.hpp"
#define GRAPHX_THEATRE_INTERPRETER

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

    gStringSettings theatreParser(std::string theatre_data);
    std::string getVariableTypeName(int variable_type);
    std::string getTheatreStructure(gStringSettings theatre_storage);

    void loadTheatre(const long TheatreID, Theatre& OutputTheatre);
    void interpretCppReference(graphx::gSettings &current_object_settings, std::string variable_name, std::string cpp_reference);
    void interpretRawData(graphx::gSettings &current_object_settings, std::string variable_name, std::string raw_data);
    void interpretExternalReference(graphx::gSettings &current_object_settings, std::string variable_name, std::string external_reference);
    void interpretTheatreReference(graphx::gSettings &current_object_settings, std::string variable_name, std::string theatre_reference, Theatre &new_theatre, gStringSettings &theatre_settings);
    void interpretSandwich(graphx::gSettings& current_object_settings, gStringSettings& theatre_settings, std::string current_object_name, int& i, int& it, unsigned long settings_size, Theatre& new_theatre);
private:

    // This is just me making the error printout easier to find and add to
    std::string what_are_the_valid_extensions =              \
        "(GraphXTheatre)\n\t" + graphx_theatre_extensions  + \
        "(3D Model)\n\t"      + three_dee_model_extensions + \
        "(Image)\n\t"         + image_extensions;

};

namespace graphx
{
    extern GraphXTheatreInterpreter Interpreter;
}
#endif