#ifndef GRAPHX_THEATRE_FILE_FORMAT
#define GRAPHX_THEATRE_FILE_FORMAT
#include "graphx_namespace.hpp"

#define RAW_DATA           1
#define CPP_REFERENCE      2
#define THEATRE_REFERENCE  3
#define EXTERNAL_REFERENCE 4
#define SANDWICH_BUN       5

// Forward Declarations
struct Theatre;

template<typename T> std::any getVariableFrom(T *object_pointer, std::string variable_name);

extern std::map<std::string, std::any> cpp_definitions;
extern bool loading_new_main_theatre;

graphx::gStringSettings theatreParser(std::string theatre_data);
graphx::gRawData 		extractData(std::string data_in_here);
std::string 			getTheatreStructure(graphx::gTheatreStorage theatre_storage);
bool					checkForAndLoadExternalTheatres();
void 					loadMainTheatre(long theatre_uid);
void 					loadChildTheatre(long theatre_uid, Theatre *parent_theatre);
int 					getClassHash(std::string class_name, bool dont_print_error = false);
#endif
