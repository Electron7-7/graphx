#include "g_math.hpp"
#include "g_actors.hpp"
#include <iostream>
#include <cmath>

namespace gmath
{
	// ModifiedBubbleSort for objects with a vao_id in them (this could be better/less hardcoded)
	template void VAO_ID_ModifiedBubbleSort<Mesh *>(std::vector<Mesh *> &unsorted_vector);
	template void VAO_ID_ModifiedBubbleSort<Actor *>(std::vector<Actor *> &unsorted_vector);

	template<typename T> void VAO_ID_ModifiedBubbleSort(std::vector<T> &unsorted_vector)
	{
		if(!unsorted_vector[0]->vao_id)
		{
			std::cerr << "ERROR::MATH::VAO_ID_MODIFIEDBUBBLESORT -> vao_id does not exist within the children of the supplied vector!" << std::endl;
			return;
		}

		int vector_size = unsorted_vector.size();
		for(int x = 0 ; x < vector_size ; x++)
		{
			bool sorted = true;
			for(int y = 0 ; y < (vector_size - 1) ; y++)
			{
				if(unsorted_vector[y]->vao_id > unsorted_vector[y + 1]->vao_id)
				{
					T temp_cmd(unsorted_vector[y + 1]);
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