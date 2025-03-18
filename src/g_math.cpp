#include <gmath.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Jolt/Jolt.h>
#include "Jolt/Core/Color.h"

gmath::uintvec3::uintvec3(unsigned int new_x, unsigned int new_y, unsigned int new_z)
: data(new_x, new_y, new_z)
{}

gmath::uintvec3::uintvec3(unsigned int new_xyz[3])
: data(new_xyz[0], new_xyz[1], new_xyz[2])
{}

gmath::uintvec3::uintvec3(unsigned int new_xyz)
: data(new_xyz, new_xyz, new_xyz)
{}

gmath::uintvec3::uintvec3(glm::vec3 glm_vector)
: data(static_cast<unsigned int>(static_cast<int>(glm_vector.x)), static_cast<unsigned int>(static_cast<int>(glm_vector.y)), static_cast<unsigned int>(static_cast<int>(glm_vector.z)))
{}

unsigned int gmath::uintvec3::x()
{
	return data[0];
}

unsigned int gmath::uintvec3::y()
{
	return data[1];
}

unsigned int gmath::uintvec3::z()
{
	return data[2];
}


template<> JPH::Vec3 gmath::convertMath(const glm::vec3 &convert_me)
{
	return JPH::Vec3(convert_me.x, convert_me.y, convert_me.z);
}

template<> glm::vec3 gmath::convertMath(const JPH::Vec3 &convert_me)
{
	return glm::vec3(convert_me.GetX(), convert_me.GetY(), convert_me.GetZ());
}

template<> glm::qua<float> gmath::convertMath(const JPH::Quat &convert_me)
{
	return glm::quat(convert_me.GetW(), convert_me.GetX(), convert_me.GetY(), convert_me.GetZ());
}

template<> JPH::Quat gmath::convertMath(const glm::quat &convert_me)
{
	return JPH::Quat(convert_me.x, convert_me.y, convert_me.z, convert_me.w);
}

template<> glm::vec3 gmath::convertMath(const JPH::Color &convert_me)
{
	return glm::vec3(convert_me.r, convert_me.g, convert_me.b);
}

template<> void gmath::convertMath(glm::vec3 &convert_me, JPH::Vec3 convert_from)
{
	convert_me = glm::vec3(convert_from.GetX(), convert_from.GetY(), convert_from.GetZ());
}

template<> void gmath::convertMath(JPH::Vec3 &convert_me, glm::vec3 convert_from)
{
	convert_me = JPH::Vec3(convert_from.x, convert_from.y, convert_from.z);
}

template<> void gmath::convertMath(glm::quat &convert_me, JPH::Quat convert_from)
{
	convert_me = glm::quat(convert_from.GetW(), convert_from.GetX(), convert_from.GetY(), convert_from.GetZ());
}

template<> void gmath::convertMath(JPH::Quat &convert_me, glm::quat convert_from)
{
	convert_me = JPH::Quat(convert_from.x, convert_from.y, convert_from.z, convert_from.w);
}

template<> void gmath::convertMath(glm::vec3 &convert_me, JPH::Color convert_from)
{
	convert_me = glm::vec3(convert_me.r, convert_me.g, convert_me.b);
}

template<> void gmath::radToDeg(JPH::Vec3 &convert_me)
{
	convert_me = JPH::Vec3(JPH::RadiansToDegrees(convert_me.GetX()), JPH::RadiansToDegrees(convert_me.GetY()), JPH::RadiansToDegrees(convert_me.GetZ()));
}

template<> void gmath::degToRad(JPH::Vec3 &convert_me)
{
	convert_me = JPH::Vec3(JPH::DegreesToRadians(convert_me.GetX()), JPH::DegreesToRadians(convert_me.GetY()), JPH::DegreesToRadians(convert_me.GetZ()));
}

template<> JPH::Vec3 gmath::linearInterpolate(JPH::Vec3 from, JPH::Vec3 to, float by)
{
	return JPH::Vec3(std::lerp(from.GetX(), to.GetX(), by), std::lerp(from.GetY(), to.GetY(), by), std::lerp(from.GetZ(), to.GetZ(), by));
}

template<> bool gmath::areEqual(int left[2], int right[2])
{
	return (left[0] == right[0] && left[1] == right[1]);
}

template<typename T, typename A> T gmath::truncateExcessive(T from, A to)
{
	static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<A>);
	double new_to = static_cast<double>(to) + 0.0001;
	double new_from = static_cast<double>(from);
	if(new_from <= new_to && new_from >= -new_to)
		return static_cast<T>(to);
	return from;
}