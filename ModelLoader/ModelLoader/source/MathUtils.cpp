#include "MathsUtils.h"

//Project Includes
#include <ctime>

/// /// <summary>
/// Seeds the RNG if neccessary
/// </summary>
void MathsUtils::SeedRandom()
{	//Seed if 1st time random has been called
	static bool isSeeded = false;
	if (!isSeeded)
	{
		srand(time(nullptr));
		isSeeded = true;
	}
}