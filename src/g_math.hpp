#ifndef GRAPHX_MATH
#define GRAPHX_MATH
#include "sanity.hpp"
#include "g_actors.hpp"
#include <vector>

namespace gmath
{
	// template <typename T> void VAO_ID_ModifiedBubbleSort(std::vector<T *> &unsorted_vector);
	void VAO_ID_ModifiedBubbleSort(std::vector<Actor *> &unsorted_vector);
	bool compareVAOID(Actor *left, Actor *right);
}
#endif