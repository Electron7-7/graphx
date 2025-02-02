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
class Theatre;
extern int current_theatre_uid;
extern std::unordered_map<int, Theatre> all_theatres;
extern std::unordered_map<std::string, std::any> interpreter_definitions;

template<typename T> T translateData(std::string data);

Theatre *loadTheatre(const char *theatre_data);
Theatre *getCurrentTheatre();
#endif