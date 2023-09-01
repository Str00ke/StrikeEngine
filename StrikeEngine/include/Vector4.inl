#include "Vector4.hpp"


template<typename T>
Vector4<T>::Vector4(T V) :
	x(V),
	y(V),
	z(V),
	w(V)
{
}

template<typename T>
Vector4<T>::Vector4(T X, T Y, T Z, T W) :
	x(X),
	y(Y),
	z(Z),
	w(W)
{
}

template<typename T>
void Vector4<T>::cross(Vector4& v1, Vector4& v2, Vector4& v3)
{
	this->x = v1.y * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.y * v3.w - v3.y * v2.w) + v1.w * (v2.y * v3.z - v2.z * v3.y);
	this->y = -(v1.x * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.z - v3.x * v2.z));
	this->z = v1.x * (v2.y * v3.w - v3.y * v2.w) - v1.y * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.y - v3.x * v2.y);
	this->w = -(v1.x * (v2.y * v3.z - v3.y * v2.z) - v1.y * (v2.x * v3.z - v3.x * v2.z) + v1.z * (v2.x * v3.y - v3.x * v2.y));
}

template<typename T>
Vector4<T> Vector4<T>::operator+(const Vector4& vec) const
{
	return Vector4{ x + vec.x, y + vec.y, z + vec.z, w + vec.w };
}

template<typename T>
Vector4<T> Vector4<T>::operator-(const Vector4& vec) const
{
	return Vector4{ x - vec.x, y - vec.y, z - vec.z, w - vec.w };
}

template<typename T>
Vector4<T> Vector4<T>::operator*(const Vector4& vec) const
{
	return Vector4{ x * vec.x, y * vec.y, z * vec.z, w * vec.w };
}

template<typename T>
Vector4<T> Vector4<T>::operator*(T value) const
{
	return Vector4{ x * value, y * value, z * value, w * value };
}

template<typename T>
Vector4<T> Vector4<T>::operator/(const Vector4& vec) const
{
	return Vector4{ x / vec.x, y / vec.y, z / vec.z, w / vec.w };
}

template<typename T>
Vector4<T> Vector4<T>::operator/(T value) const
{
	return Vector4{ x / value, y / value, z / value, w / value };
}

//template<typename T>
//Vector4<T>& Vector4<T>::operator+=(const Vector4& vec)
//{
//	x += vec.x;
//	y += vec.y;
//	z += vec.z;
//	w += vec.w;
//
//	return *this;
//}
//
//template<typename T>
//Vector4<T>& Vector4<T>::operator-=(const Vector4& vec)
//{
//	x -= vec.x;
//	y -= vec.y;
//	z -= vec.z;
//	w -= vec.w;
//
//	return *this;
//}
//
//template<typename T>
//Vector4<T>& Vector4<T>::operator*=(const Vector4& vec)
//{
//	x *= vec.x;
//	y *= vec.y;
//	z *= vec.z;
//	w *= vec.w;
//
//	return *this;
//}
//
//template<typename T>
//Vector4<T>& Vector4<T>::operator*=(T value)
//{
//	x *= value;
//	y *= value;
//	z *= value;
//	w *= value;
//
//	return *this;
//}
//
//template<typename T>
//Vector4<T>& Vector4<T>::operator/=(const Vector4& vec)
//{
//	x /= vec.x;
//	y /= vec.y;
//	z /= vec.z;
//	w /= vec.w;
//
//	return *this;
//}
//
//template<typename T>
//Vector4<T>& Vector4<T>::operator/=(T value)
//{
//	x /= value;
//	y /= value;
//	z /= value;
//	w /= value;
//
//	return *this;
//}

template<typename T>
Vector4<T> operator*(T value, const Vector4<T>& vec)
{
	return Vector4{ vec.x * value, vec.y * value, vec.z * value, vec.w * value };
}

template<typename T>
Vector4<T> operator/(T value, const Vector4<T>& vec)
{
	return Vector4{ vec.x / value, vec.y / value, vec.z / value, vec.w / value };
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector4<T>& vec)
{
	return os << "Vector4(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
}