#pragma once
#include <ostream>
#include "Math/Vector3.hpp"

namespace StrikeEngine
{


}
//No export macro for templates
template<typename T>
struct Vector4
{
	Vector4() = default; //non-init value
	/*Vector4(const Vector3& vec) :
		x(vec.x),
		y(vec.y),
		z(vec.z),
		w(1.0f)
	{}*/
	explicit Vector4(T V); //Forbid implicit conversion, like Vector4 vec = 5;
	Vector4(T X, T Y, T Z, T W);

	Vector4 operator+(const Vector4& vec) const;
	Vector4 operator-(const Vector4& vec) const;
	Vector4 operator*(const Vector4& vec) const;
	Vector4 operator*(T value) const;
	Vector4 operator/(const Vector4& vec) const;
	Vector4 operator/(T value) const;

	/*Vector4& operator+=(const Vector4& vec) const;
	Vector4& operator-=(const Vector4& vec) const;
	Vector4& operator*=(const Vector4& vec) const;
	Vector4& operator*=(T value) const;
	Vector4& operator/=(const Vector4& vec) const;
	Vector4& operator/=(T value) const;*/

	void cross(Vector4& v1, Vector4& v2, Vector4& v3);

	T x, y, z, w;
};

template<typename T> Vector4<T> operator*(T value, const Vector4<T>& vec);
template<typename T> Vector4<T> operator/(T value, const Vector4<T>& vec);

//Stream operator, directly write vector in console
template<typename T> std::ostream& operator<<(std::ostream& os, const Vector4<T>& vec);

using Vector4f = Vector4<float>;
using Vector4i = Vector4<int>;

#include "Math/Vector4.inl"