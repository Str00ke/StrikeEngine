#pragma once
#include "Vector3.hpp"
#include "Core/defines.hpp"

namespace StrikeEngine
{
#define PI 3.14159265358979323846264338327950288419716939937510582
#define PIF 3.141592f

	static class Math
	{
	public:
		template<typename T>
		static T Min(T firstValue, T secondValue);

		template<typename T>
		static T Max(T firstValue, T secondValue);

		static float Rad2Deg(float radians);
		static float Deg2Rad(float degrees);

		template<typename T>
		static T Clamp(T minValue, T maxValue, T value)
		{
			if (value < minValue) return minValue;
			else if (value > maxValue) return maxValue;
			else return value;
		}

		template<typename T>
		static void Clamp(T& minValue, T& maxValue, T& value)
		{
			if (value < minValue) value = minValue;
			else if (value > maxValue) value = maxValue;
		}

		// TODO: Impl
		static u32 ceil(f32 _value);
		static u32 floor(f32 _value);
		static u32 toNearest(f32 _value);

		template<typename T>
		static T Dot(Vector3<T> u, Vector3<T> v);

		template<typename T>
		static Vector3<T> Cross(Vector3<T> u, Vector3<T> v);
	};

	
	


}
