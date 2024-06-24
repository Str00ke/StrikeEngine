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

	template<typename T>
	Vector3<T>Math::Cross(Vector3<T> u, Vector3<T> v)
	{
		return Vector3<T>(
			(u.y * v.z) - (u.z * v.y),
			(u.z * v.x) - (u.x * v.z),
			(u.x * v.y) - (u.y * v.z));
	}

	template<typename T>
	T Math::Dot(Vector3<T> u, Vector3<T> v)
	{
		return ((u.x * v.x) + (u.y * v.y) + (u.z * v.z));
	}
}