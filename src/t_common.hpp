#ifndef GRAPHX_THEATRE_FILE_FORMAT
#define GRAPHX_THEATRE_FILE_FORMAT
// #include "g_common.hpp"
// #include "g_jolt.hpp"
#include "cube.graphxmodel"
#include "ERROR.graphxmodel"
#include "pyramid.graphxmodel"
#include "quad.graphxmodel"
#include <iostream>
#include <fstream>
#include <sstream>
#include <any>
#include <unordered_map>
extern int current_theatre_uid;
// extern std::unordered_map<int, Theatre> all_theatres;
extern std::unordered_map<std::string, int> graphx_class_names;

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

// void createNewClass(int class_uid, std::string object_name);
template<typename T> T translateData(std::string data);
int loadTheatre(std::string embedded_theatre);
void theatreParser(std::string theatre_data);
std::string getTheatreStructure();
int getClassHash(std::string &class_name);
#endif