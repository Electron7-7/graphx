#ifndef GRAPHX_MATH
#define GRAPHX_MATH

template<typename T, typename A> T convertMath(const A &convert_me);
template<typename T, typename A> void convertMath(A &convert_me, T convert_from);
template<typename T> void radToDeg(T &convert_me);
template<typename T> void degToRad(T &convert_me);
template<typename T> T linearInterpolate(T from, T to, float by);
template<typename T> bool areEqual(T left, T right);
template<typename T, typename A> T truncateExcessive(T from, A to);
#endif