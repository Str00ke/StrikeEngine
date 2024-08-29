#pragma once
#include "Math/Math.hpp"

template<typename T>
Vector3<T> Vector3<T>::operator+(const Vector3& vec) const
{
	return Vector3{ x + vec.x, y + vec.y, z + vec.z };
}

template<typename T>
T Clamp(T minValue, T maxValue, T value) 
{
	if (value < minValue) return minValue;
	else if (value > maxValue) return maxValue;
	else return value;
}