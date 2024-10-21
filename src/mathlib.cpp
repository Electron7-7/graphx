#include <mathlib.h>

float J_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = * ( long * ) &y;					// evil floating point bit level hacking (say the line, Bart)
	i = 0x5f3759df - ( i >> 1 );			// what the fuck? (based af)
	y = * ( float * ) &i;
	y = y * ( threehalfs - ( x2 * y * y) );	// 1st iteration (2nd iteration was removed!)

	return y;
}