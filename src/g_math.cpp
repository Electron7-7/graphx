#include "g_math.hpp"

template<> JPH::Vec3 convertMath(const glm::vec3 &convert_me)
{
	return JPH::Vec3(convert_me[0], convert_me[1], convert_me[2]);
}

template<> JPH::Vec4 convertMath(const glm::vec4 &convert_me)
{
	return JPH::Vec4(convert_me[0], convert_me[1], convert_me[2], convert_me[3]);
}

template<> glm::vec3 convertMath(const JPH::Vec3 &convert_me)
{
	return glm::vec3(convert_me[0], convert_me[1], convert_me[2]);
}

template<> glm::vec4 convertMath(const JPH::Vec4 &convert_me)
{
	return glm::vec4(convert_me[0], convert_me[1], convert_me[2], convert_me[3]);
}

template<> glm::qua<float> convertMath(const JPH::Quat &convert_me)
{
	return glm::quat(convert_me.GetW(), convert_me.GetX(), convert_me.GetY(), convert_me.GetZ());
}

template<> JPH::Quat convertMath(const glm::quat &convert_me)
{
	return JPH::Quat(convert_me[3], convert_me[0], convert_me[1], convert_me[2]);
}

template<> JPH::Vec3 linearInterpolate(JPH::Vec3 from, JPH::Vec3 to, float by)
{
	return JPH::Vec3(std::lerp(from.GetX(), to.GetX(), by), std::lerp(from.GetY(), to.GetY(), by), std::lerp(from.GetZ(), to.GetZ(), by));
}

template<> bool areEqual(int left[2], int right[2])
{
	return (left[0] == right[0] && left[1] == right[1]);
}

template<typename T, typename A> T truncateExcessive(T from, A to)
{
	static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<A>);
	double new_to = static_cast<double>(to) + 0.0001;
	double new_from = static_cast<double>(from);
	if(new_from <= new_to && new_from >= -new_to)
		return static_cast<T>(to);
	return from;
}