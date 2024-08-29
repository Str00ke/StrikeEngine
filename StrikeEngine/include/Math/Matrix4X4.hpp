#pragma once
#include "Math/Vector3.hpp"
#include "Math/Vector4.hpp"

namespace StrikeEngine
{
	class Matrix4X4
	{
	public:
		Matrix4X4();
		//Matrix4X4(const Matrix4X4&) = delete;
		//Matrix4X4(Matrix4X4&&) = delete;
		~Matrix4X4()
		{};
		void Identity();
		void SetTranslation(const Vector3f& translation);
		void SetRotationX(const float x);
		void SetRotationY(const float y);
		void SetRotationZ(const float z);
		void SetScale(const Vector3f& scale);
		void SetRaw(const Vector4f& _row1, const Vector4f& _row2, const Vector4f& _row3, const Vector4f& _row4);

		void SetPerspectiveProjectionMatrix(const float aspectRatio, const float fieldOfView, const float nearClip, const float farClip);
		void SetOrthographicProjectionMatrix(const float leftPlane, const float rightPlane, const float topPlane, const float bottomPlane, const float nearPlane, const float farPlane);

		float GetDeterminant();

		void Inverse();

		Vector3f GetZDirection();
		Vector3f GetXDirection();
		Vector3f GetTranslation();

		static Matrix4X4& GetPerspectiveProjectionMatrix(const float aspectRatio, const float fieldOfView, const float nearClip, const float farClip);
		static Matrix4X4& GetOrthographicProjectionMatrix(const float leftPlane, const float rightPlane, const float topPlane, const float bottomPlane, const float nearPlane, const float farPlane);

		const float** GetMatrix();

		void operator *=(const Matrix4X4& matrix);
		Matrix4X4 operator *(const Matrix4X4& _other);

	public:
		float m_mat[4][4] = {};

	private:
		void SetMatrix(const Matrix4X4& matrix);
	};
}



