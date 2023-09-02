#include <Vector2.hpp>
//#include "Math.hpp"

template<typename T>
Vector2<T>::Vector2(T V) :
	x(V),
	y(V)
{
}

template<typename T>
Vector2<T>::Vector2(T X, T Y) :
	x(X),
	y(Y)
{
}

template<typename T>
Vector2<T> Vector2<T>::operator+(const Vector2& vec) const
{
	return Vector2{ x + vec.x, y + vec.y };
}

template<typename T>
Vector2<T> Vector2<T>::operator-(const Vector2& vec) const
{
	return Vector2{ x - vec.x, y - vec.y };
}

template<typename T>
Vector2<T> Vector2<T>::operator*(const Vector2& vec) const
{
	return Vector2{ x * vec.x, y * vec.y };
}

template<typename T>
Vector2<T> Vector2<T>::operator*(T value) const
{
	return Vector2{ x * value, y * value };
}

template<typename T>
Vector2<T> Vector2<T>::operator/(const Vector2& vec) const
{
	return Vector2{ x / vec.x, y / vec.y };
}

template<typename T>
Vector2<T> Vector2<T>::operator/(T value) const
{
	return Vector2{ x / value, y / value };
}

template<typename T>
Vector2<T>& Vector2<T>::operator+=(const Vector2& vec)
{
	x += vec.x;
	y += vec.y;

	return *this;
}

template<typename T>
Vector2<T>& Vector2<T>::operator-=(const Vector2& vec)
{
	x -= vec.x;
	y -= vec.y;

	return *this;
}

template<typename T>
Vector2<T>& Vector2<T>::operator*=(const Vector2& vec)
{
	x *= vec.x;
	y *= vec.y;

	return *this;
}

template<typename T>
Vector2<T>& Vector2<T>::operator*=(T value)
{
	x *= value;
	y *= value;

	return *this;
}

template<typename T>
Vector2<T>& Vector2<T>::operator/=(const Vector2& vec)
{
	x /= vec.x;
	y /= vec.y;

	return *this;
}

template<typename T>
Vector2<T>& Vector2<T>::operator/=(T value)
{
	x /= value;
	y /= value;

	return *this;
}

//template<typename T>
//Vector2<T> Vector2<T>::Rotate(const Vector2& vec, float degrees)
//{
//	float radRotation = Math::Deg2Rad(degrees);
//	float s = std::sin(radRotation);
//	float c = std::cos(radRotation);
//
//	Vector2 rotatedVec;
//	rotatedVec.x = vec.x * c - vec.y * s;
//	rotatedVec.y = vec.x * s + vec.y * c;
//
//	return rotatedVec;
//}

template<typename T>
Vector2<T> operator*(T value, const Vector2<T>& vec)
{
	return Vector2{ vec.x * value, vec.y * value };
}

template<typename T>
Vector2<T> operator/(T value, const Vector2<T>& vec)
{
	return Vector2{ vec.x / value, vec.y / value };
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector2<T>& vec)
{
	return os << "Vector2(" << vec.x << ", " << vec.y << ")";
}