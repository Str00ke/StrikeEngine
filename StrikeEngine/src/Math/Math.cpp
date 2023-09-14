#include "Math/Math.hpp"

namespace StrikeEngine
{
	template<typename T>
	T Math::Max(T firstValue, T secondValue)
	{
		return firstValue > secondValue ? firstValue : secondValue;
	}

	float Math::Rad2Deg(float radians)
	{
		return radians * (180 / PI);
	}

	float Math::Deg2Rad(float degrees)
	{
		return degrees * (PI * 180);
	}

	template<typename T>
	T Math::Min(T firstValue, T secondValue)
	{
		return firstValue < secondValue ? firstValue : secondValue;
	}

	template<typename T>
	T Math::Clamp(T minValue, T maxValue, T value)
	{
		if (value < minValue) return minValue;
		else if (value > maxValue) return maxValue;
		else return value;
	}
}