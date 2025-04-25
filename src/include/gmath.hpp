#ifndef GRAPHX_MATH
#define GRAPHX_MATH
#include <glm/fwd.hpp>
namespace gmath
{
	struct vec3uint
	{
	public:
		unsigned int data[3];

		vec3uint(unsigned int new_x = 0, unsigned int new_y = 0, unsigned int new_z = 0);
		vec3uint(unsigned int new_xyz[3]);
		vec3uint(unsigned int new_xyz);
		vec3uint(glm::vec3 glm_vector);

		unsigned int x();
		unsigned int y();
		unsigned int z();
	};

	// struct vec3f
	// {
	// public:
	// 	float data[3];
	// };

	template<typename T, typename A> T convertMath(const A &convert_me);
	template<typename T, typename A> void convertMath(A &convert_me, T convert_from);
	template<typename T> void radToDeg(T &convert_me);
	template<typename T> void degToRad(T &convert_me);
	template<typename T> T linearInterpolate(T from, T to, float by);
	template<typename T> bool areEqual(T left, T right);
	template<typename T, typename A> T truncateExcessive(T from, A to);
};
#endif