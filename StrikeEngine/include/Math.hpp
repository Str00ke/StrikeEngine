#pragma once

namespace StrikeEngine
{
#define PI 3.14159265358979323846264338327950288419716939937510582

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
		static T Clamp(T minValue, T maxValue, T value);
	};

	


}


