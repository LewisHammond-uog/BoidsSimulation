#ifndef __MATHS_UTILS_H__
#define __MATHS_UTILS_H__

#include <random>

/// <summary>
/// Maths utility static class
/// Class contains a number of useful maths functions usable from any where in the
/// application
/// </summary>
class MathsUtils
{
public:
	template<class T>
	static T RandomRange(T a_rangeStart, T a_rangeEnd);
};

template <class T>
T MathsUtils::RandomRange(T a_rangeStart, T a_rangeEnd)
{
	return a_rangeStart + T(rand() / T(RAND_MAX) * (a_rangeEnd - a_rangeStart));
}


#endif //!__MATHS_UTILS_H__
