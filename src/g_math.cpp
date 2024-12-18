#include "g_math.hpp"

namespace gmath
{
	bool compareVAOID(Mesh *left, Mesh *right)
	{
		return (left->vao_id > right->vao_id);
	}
}