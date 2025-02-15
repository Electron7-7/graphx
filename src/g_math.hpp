#ifndef GRAPHX_MATH
#define GRAPHX_MATH
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Jolt/Jolt.h>

template<typename T, typename A> T convertMath(const A &convert_me);
template<typename T> T linearInterpolate(T from, T to, float by);
#endif