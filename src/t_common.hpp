#ifndef GRAPHX_THEATRE_FILE_FORMAT
#define GRAPHX_THEATRE_FILE_FORMAT
#include "graphx_namespace.hpp"

#define RAW_DATA           0
#define CPP_REFERENCE      1
#define THEATRE_REFERENCE  2
#define EXTERNAL_REFERENCE 3
#define SANDWICH_BUN       4

// Forward Declarations
struct Theatre;

template<typename T> std::any getVariableFrom(T *object_pointer, std::string variable_name);

extern bool loading_new_main_theatre;
extern std::map<std::string, std::any> cpp_definitions;

graphx::gStringSettings theatreParser(std::string theatre_data);
graphx::gRawData 		extractData(std::string data_in_here);
std::string 			getTheatreStructure(graphx::gTheatreStorage theatre_storage);
bool					checkForAndLoadExternalTheatres();
void 					loadMainTheatre(long theatre_uid);
void 					loadChildTheatre(long theatre_uid, Theatre *parent_theatre);
int 					getClassHash(std::string class_name, bool dont_print_error = false);
void 					interpretCppReference(graphx::gSettings &new_class_settings, std::string variable_final_name, std::string variable_name, std::string cpp_reference);
void 					interpretRawData(graphx::gSettings &new_class_settings, std::string variable_final_name, std::string variable_name, std::string raw_data);
void 					interpretTheatreReference(graphx::gSettings &new_class_settings, std::string variable_final_name, std::string variable_name, std::string theatre_reference, Theatre *new_theatre);
void 					interpretExternalReference(graphx::gSettings &new_class_settings, std::string variable_final_name, std::string variable_name, std::string external_reference);
#endif