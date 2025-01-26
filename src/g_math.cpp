#include "g_math.hpp"

template<> JPH::Vec3 convertMath(glm::vec3 &convert_me)
{
	return JPH::Vec3(convert_me[0], convert_me[1], convert_me[2]);
}

template<> JPH::Vec4 convertMath(glm::vec4 &convert_me)
{
	return JPH::Vec4(convert_me[0], convert_me[1], convert_me[2], convert_me[3]);
}

template<> glm::vec3 convertMath(JPH::Vec3 &convert_me)
{
	return glm::vec3(convert_me[0], convert_me[1], convert_me[2]);
}

template<> glm::vec4 convertMath(JPH::Vec4 &convert_me)
{
	return glm::vec4(convert_me[0], convert_me[1], convert_me[2], convert_me[3]);
}

template<> glm::qua<float> convertMath(JPH::Quat &convert_me)
{
	return glm::quat(convert_me.GetW(), convert_me.GetX(), convert_me.GetY(), convert_me.GetZ());
}

template<> JPH::Quat convertMath(glm::quat &convert_me)
{
	return JPH::Quat(convert_me[3], convert_me[0], convert_me[1], convert_me[2]);
}