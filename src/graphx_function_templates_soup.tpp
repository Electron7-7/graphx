#ifndef GRAPHX_FUNCTION_TEMPLATES_SOUP
#define GRAPHX_FUNCTION_TEMPLATES_SOUP
#include "g_common.hpp"
#include "r_common.hpp"
template<typename T> Actor *createNewActor() { return new T; }
template<typename T> Device *createNewDevice() { return new T; }
#endif