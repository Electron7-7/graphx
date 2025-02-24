#ifndef GRAPHX_THEATRE_FILE_FORMAT
#define GRAPHX_THEATRE_FILE_FORMAT
#include "graphx_namespace.hpp"

// Forward Declarations
struct Theatre;

template<typename T> std::any getVariableFrom(T *object_pointer, std::string variable_name);

extern std::map<std::string, std::any> cpp_definitions;
extern bool loading_new_main_theatre;

graphx::gTheatreStorage theatreParser(std::string theatre_data);
graphx::gRawData 		extractData(std::string data_in_here);
std::string 			getTheatreStructure(graphx::gTheatreStorage theatre_storage);
bool					checkForAndLoadExternalTheatres();
void 					loadMainTheatre(long theatre_uid);
void 					loadChildTheatre(long theatre_uid, Theatre *parent_theatre);
int 					getClassHash(std::string class_name, bool dont_print_error = false);
#endif
