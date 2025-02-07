#ifndef GRAPHX_THEATRE_FILE_FORMAT
#define GRAPHX_THEATRE_FILE_FORMAT
#include "g_common.hpp"
// #include "g_jolt.hpp"
#include <map>
#include <any>
#include <tuple>
#include <string>
#include <vector>
#include <unordered_map>
extern int current_theatre_uid;
extern std::unordered_map<int, Theatre> all_theatres;
extern std::unordered_map<std::string, int> graphx_class_names;
extern std::unordered_map<std::string, std::any> cpp_definitions;

namespace graphx_classes
{
	static constexpr int THEATRE		= 0;
	static constexpr int ACTOR			= 1;
	static constexpr int RIGIDBODYACTOR	= 2;
	static constexpr int COLLIDER		= 3;
	static constexpr int MESH			= 4;
	static constexpr int MATERIAL		= 5;
	static constexpr int LIGHT			= 6;
};

typedef std::map<int, std::pair<std::string, std::string>>														gObjectStore;
typedef std::multimap<int, std::pair<std::string, std::string>>													gSourceRefStore;
typedef std::multimap<int, std::pair<std::string, int>>															gTheatreRefStore;
typedef std::multimap<int, std::pair<std::string, std::string>>													gRawDataStore;
typedef std::multimap<int, std::pair<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>>>	gSandwichStore;
typedef std::tuple<std::string, gObjectStore, gSourceRefStore, gTheatreRefStore, gRawDataStore, gSandwichStore>	gTheatreStorage;
typedef std::unordered_map<std::string, std::any> 																gSettings;

extern gSettings actor_settings;

template<typename T> std::any getVariableFrom(T *object_pointer, std::string variable_name);
template<typename T> std::any getNumber(std::vector<std::string> string_input);

std::any extractData(std::string data_in_here);
std::any snagCppData(std::string reference);
gSettings getSettingsTemplate(std::string class_name);
void createNewClass(int class_uid, gTheatreStorage *theatre_data);
int loadTheatre(std::string embedded_theatre);
gTheatreStorage theatreParser(std::string theatre_data);
std::string getTheatreStructure(gTheatreStorage theatre_storage);
int getClassHash(std::string class_name);
#endif