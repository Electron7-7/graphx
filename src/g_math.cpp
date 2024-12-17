#include "sanity.hpp"
#include "g_math.hpp"
#include "g_actors.hpp"
#include <cmath>

namespace gmath
{
	bool compareVAOID(Actor *left, Actor *right) { return (*left->vao_id > *right->vao_id); }
	
	// template void VAO_ID_ModifiedBubbleSort(std::vector<Actor *> &unsorted_vector);

	void VAO_ID_ModifiedBubbleSort(std::vector<Actor *> &unsorted_vector)
	{
		int vector_size = unsorted_vector.size();
		for(int x = 0 ; x < vector_size ; x++)
		{
			bool sorted = true;
			for(int y = 0 ; y < (vector_size - 1) ; y++)
			{
				if(unsorted_vector[y]->vao_id > unsorted_vector[y + 1]->vao_id)
				{
					PRINT_MARKER;
					Actor *temp_cmd = unsorted_vector[y + 1];
					unsorted_vector[y + 1] = unsorted_vector[y];
					unsorted_vector[y] = temp_cmd;
					sorted = false;
				}
			}
			if(sorted)
				break;
		}
	}
}