#include "g_math.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Jolt/Jolt.h>

template<> JPH::Vec3 convertMath(const glm::vec3 &convert_me)
{
	return JPH::Vec3(convert_me.x, convert_me.y, convert_me.z);
}

template<> glm::vec3 convertMath(const JPH::Vec3 &convert_me)
{
	return glm::vec3(convert_me.GetX(), convert_me.GetY(), convert_me.GetZ());
}

template<> glm::qua<float> convertMath(const JPH::Quat &convert_me)
{
	return glm::quat(convert_me.GetW(), convert_me.GetX(), convert_me.GetY(), convert_me.GetZ());
}

template<> JPH::Quat convertMath(const glm::quat &convert_me)
{
	return JPH::Quat(convert_me.x, convert_me.y, convert_me.z, convert_me.w);
}

template<> void convertMath(glm::vec3 &convert_me, JPH::Vec3 convert_from)
{
	convert_me = glm::vec3(convert_from.GetX(), convert_from.GetY(), convert_from.GetZ());
}

template<> void convertMath(JPH::Vec3 &convert_me, glm::vec3 convert_from)
{
	convert_me = JPH::Vec3(convert_from.x, convert_from.y, convert_from.z);
}

template<> void convertMath(glm::quat &convert_me, JPH::Quat convert_from)
{
	convert_me = glm::quat(convert_from.GetW(), convert_from.GetX(), convert_from.GetY(), convert_from.GetZ());
}

template<> void convertMath(JPH::Quat &convert_me, glm::quat convert_from)
{
	convert_me = JPH::Quat(convert_from.x, convert_from.y, convert_from.z, convert_from.w);
}

template<> void radToDeg(JPH::Vec3 &convert_me)
{
	convert_me = JPH::Vec3(JPH::RadiansToDegrees(convert_me.GetX()), JPH::RadiansToDegrees(convert_me.GetY()), JPH::RadiansToDegrees(convert_me.GetZ()));
}

template<> void degToRad(JPH::Vec3 &convert_me)
{
	convert_me = JPH::Vec3(JPH::DegreesToRadians(convert_me.GetX()), JPH::DegreesToRadians(convert_me.GetY()), JPH::DegreesToRadians(convert_me.GetZ()));
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