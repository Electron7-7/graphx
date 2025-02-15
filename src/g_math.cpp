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
	return JPH::Vec3(std::lerp(from[0], to[0], by), std::lerp(from[1], to[1], by), std::lerp(from[2], to[2], by));
}