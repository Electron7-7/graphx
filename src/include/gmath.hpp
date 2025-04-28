#ifndef GRAPHX_MATH
#include <glm/fwd.hpp>
#include <vector>
#define GRAPHX_MATH
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

	template<typename T, typename A> T convertMath(const A& ConvertMe);
	template<typename T, typename A> void convertMath(A& StoreConversion, T ConvertMe);
	template<typename T> void radToDeg(T& ConvertMe);
	template<typename T> void degToRad(T& ConvertMe);
	template<typename T> T linearInterpolate(T From, T To, float ByAmount);
	template<typename T> bool areEqual(T Left, T Right);
	template<typename T, typename A> T truncateExcessive(T From, A To);
	template<typename T> std::vector<float> glmVectorToFloats(const T& Vector);
};
#endif