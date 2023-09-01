#pragma once
#include "Vector3.hpp"

namespace StrikeEngine
{
	class Matrix4X4
	{
	public:
		Matrix4X4();
		Matrix4X4(const Matrix4X4&) = delete;
		//Matrix4X4(Matrix4X4&&) = delete;
		~Matrix4X4()
		{};
		void Identity();
		void SetTranslation(const Vector3f& translation);
		void SetRotationX(float x);
		void SetRotationY(float y);
		void SetRotationZ(float z);
		void SetScale(const Vector3f& scale);

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

	public:
		float m_mat[4][4] = {};

	private:
		void SetMatrix(const Matrix4X4& matrix);
	};
}



