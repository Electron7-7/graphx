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

template<typename T> T translateData(std::string data);
void createNewClass(int class_uid, std::string object_name);

// int loadTheatre(std::string theatre_file_path);
int loadTheatre(std::string embedded_theatre);
void theatreParser(std::string theatre_data);
#endif