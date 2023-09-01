#include "Matrix4X4.hpp"
#include "Vector4.hpp"
#include <cmath>

namespace StrikeEngine
{

	Matrix4X4::Matrix4X4()
	{
		m_mat[0][0] = 0.0f;
		m_mat[0][1] = 0.0f;
		m_mat[0][2] = 0.0f;
		m_mat[0][3] = 0.0f;

		m_mat[1][0] = 0.0f;
		m_mat[1][1] = 0.0f;
		m_mat[1][2] = 0.0f;
		m_mat[1][3] = 0.0f;

		m_mat[2][0] = 0.0f;
		m_mat[2][1] = 0.0f;
		m_mat[2][2] = 0.0f;
		m_mat[2][3] = 0.0f;
	}

	void Matrix4X4::Identity()
	{
		memset(m_mat, 0, sizeof(float) * 16);
		m_mat[0][0] = 1.0f;
		m_mat[1][1] = 1.0f;
		m_mat[2][2] = 1.0f;
		m_mat[3][3] = 1.0f;
	}

	void Matrix4X4::SetTranslation(const Vector3f& translation)
	{
		m_mat[3][0] = translation.x;
		m_mat[3][1] = translation.y;
		m_mat[3][2] = translation.z;
	}

	void Matrix4X4::SetRotationX(float x)
	{
		m_mat[1][1] = cos(x);
		m_mat[1][2] = sin(x);
		m_mat[2][1] = -sin(x);
		m_mat[2][2] = cos(x);
	}

	void Matrix4X4::SetRotationY(float y)
	{
		m_mat[0][0] = cos(y);
		m_mat[0][2] = -sin(y);
		m_mat[2][0] = sin(y);
		m_mat[2][2] = cos(y);
	}

	void Matrix4X4::SetRotationZ(float z)
	{
		m_mat[0][0] = cos(z);
		m_mat[0][1] = sin(z);
		m_mat[1][0] = -sin(z);
		m_mat[1][1] = cos(z);
	}

	void Matrix4X4::SetScale(const Vector3f& scale)
	{
		m_mat[0][0] = scale.x;
		m_mat[1][1] = scale.y;
		m_mat[2][2] = scale.z;
	}

	void Matrix4X4::SetPerspectiveProjectionMatrix(const float aspectRatio, const float fieldOfView, const float nearClip, const float farClip)
	{
		float yscale = 1.0f / tan(fieldOfView / 2.0f);
		float xscale = yscale / aspectRatio;
		m_mat[0][0] = xscale;
		m_mat[1][1] = yscale;
		m_mat[2][2] = farClip / (farClip - nearClip);
		m_mat[2][3] = 1.0f;
		m_mat[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	}

	void Matrix4X4::SetOrthographicProjectionMatrix(const float leftPlane, const float rightPlane, const float topPlane, const float bottomPlane, const float nearPlane, const float farPlane)
	{
		Identity();
		m_mat[0][0] = 2.0f / (rightPlane - leftPlane);
		m_mat[1][1] = 2.0f / (bottomPlane - topPlane);
		m_mat[2][2] = 1.0f / (nearPlane - farPlane);
		m_mat[3][2] = -(nearPlane / (farPlane - nearPlane));

	}

	float Matrix4X4::GetDeterminant()
	{
		Vector4f minor, v1, v2, v3;
		float det;

		v1 = Vector4f(this->m_mat[0][0], this->m_mat[1][0], this->m_mat[2][0], this->m_mat[3][0]);
		v2 = Vector4f(this->m_mat[0][1], this->m_mat[1][1], this->m_mat[2][1], this->m_mat[3][1]);
		v3 = Vector4f(this->m_mat[0][2], this->m_mat[1][2], this->m_mat[2][2], this->m_mat[3][2]);

		minor.cross(v1, v2, v3);
		det = -(this->m_mat[0][3] * minor.x + this->m_mat[1][3] * minor.y + this->m_mat[2][3] * minor.z +
				this->m_mat[3][3] * minor.w);

		return det;
	}

	void Matrix4X4::Inverse()
	{
		/*int a, i, j;
		Matrix4X4 out;
		Vector4f v, vec[3];
		float det = 0.0f;

		det = this->GetDeterminant();
		if (!det) return;
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (j != 1)
				{
					a = j;
					if (j > i) a = a - 1;
					vec[a].x = (this->m_mat[j][0]);
					vec[a].y = (this->m_mat[j][1]);
					vec[a].z = (this->m_mat[j][2]);
					vec[a].w = (this->m_mat[j][3]);
				}
			}

			v.cross(vec[0], vec[1], vec[2]);

			out.m_mat[0][i] = pow(-1.0f, i) * v.x / det;
			out.m_mat[1][i] = pow(-1.0f, i) * v.y / det;
			out.m_mat[2][i] = pow(-1.0f, i) * v.z / det;
			out.m_mat[3][i] = pow(-1.0f, i) * v.w / det;
		}

		this->SetMatrix(out);*/
		double Result[4][4];
		double tmp[12]; /* temp array for pairs */
		double src[16]; /* array of transpose source matrix */
		double det; /* determinant */
		/* transpose matrix */
		for (uint32_t i = 0; i < 4; i++)
		{
			src[i + 0] = m_mat[i][0];
			src[i + 4] = m_mat[i][1];
			src[i + 8] = m_mat[i][2];
			src[i + 12] = m_mat[i][3];
		}
		/* calculate pairs for first 8 elements (cofactors) */
		tmp[0] = src[10] * src[15];
		tmp[1] = src[11] * src[14];
		tmp[2] = src[9] * src[15];
		tmp[3] = src[11] * src[13];
		tmp[4] = src[9] * src[14];
		tmp[5] = src[10] * src[13];
		tmp[6] = src[8] * src[15];
		tmp[7] = src[11] * src[12];
		tmp[8] = src[8] * src[14];
		tmp[9] = src[10] * src[12];
		tmp[10] = src[8] * src[13];
		tmp[11] = src[9] * src[12];
		/* calculate first 8 elements (cofactors) */
		Result[0][0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
		Result[0][0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
		Result[0][1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
		Result[0][1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
		Result[0][2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
		Result[0][2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
		Result[0][3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
		Result[0][3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
		Result[1][0] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
		Result[1][0] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
		Result[1][1] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
		Result[1][1] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
		Result[1][2] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
		Result[1][2] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
		Result[1][3] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
		Result[1][3] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];
		/* calculate pairs for second 8 elements (cofactors) */
		tmp[0] = src[2] * src[7];
		tmp[1] = src[3] * src[6];
		tmp[2] = src[1] * src[7];
		tmp[3] = src[3] * src[5];
		tmp[4] = src[1] * src[6];
		tmp[5] = src[2] * src[5];

		tmp[6] = src[0] * src[7];
		tmp[7] = src[3] * src[4];
		tmp[8] = src[0] * src[6];
		tmp[9] = src[2] * src[4];
		tmp[10] = src[0] * src[5];
		tmp[11] = src[1] * src[4];
		/* calculate second 8 elements (cofactors) */
		Result[2][0] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
		Result[2][0] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
		Result[2][1] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
		Result[2][1] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
		Result[2][2] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
		Result[2][2] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
		Result[2][3] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
		Result[2][3] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
		Result[3][0] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
		Result[3][0] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
		Result[3][1] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
		Result[3][1] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
		Result[3][2] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
		Result[3][2] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
		Result[3][3] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
		Result[3][3] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];
		/* calculate determinant */
		det = src[0] * Result[0][0] + src[1] * Result[0][1] + src[2] * Result[0][2] + src[3] * Result[0][3];
		/* calculate matrix inverse */
		det = 1.0f / det;

		Matrix4X4 FloatResult;
		for (uint32_t i = 0; i < 4; i++)
		{
			for (uint32_t j = 0; j < 4; j++)
			{
				FloatResult.m_mat[i][j] = float(Result[i][j] * det);
			}
		}
		this->SetMatrix(FloatResult);
	}

	Vector3f Matrix4X4::GetZDirection()
	{
		return Vector3f(m_mat[2][0], m_mat[2][1], m_mat[2][2]);
	}

	Vector3f Matrix4X4::GetXDirection()
	{
		return Vector3f(m_mat[0][0], m_mat[0][1], m_mat[0][2]);
	}

	Vector3f Matrix4X4::GetTranslation()
	{
		return Vector3f(m_mat[3][0], m_mat[3][1], m_mat[3][2]);
	}


	void Matrix4X4::SetMatrix(const Matrix4X4& matrix)
	{
		::memcpy(m_mat, matrix.m_mat, sizeof(float) * 16);
	}

	Matrix4X4& Matrix4X4::GetPerspectiveProjectionMatrix(const float aspectRatio, const float fieldOfView, const float nearClip, const float farClip)
	{
		Matrix4X4 mat;
		float f = 1.0f / std::tan(fieldOfView * 0.5f * 0.01745329251994329576923690768489f);
		mat.m_mat[0][0] = f / aspectRatio;
		mat.m_mat[1][1] = -f;
		mat.m_mat[2][2] = farClip / (nearClip - farClip);
		mat.m_mat[3][3] = (nearClip * farClip) / (nearClip - farClip);
		return mat;
	}

	Matrix4X4& Matrix4X4::GetOrthographicProjectionMatrix(const float leftPlane, const float rightPlane, const float topPlane, const float bottomPlane, const float nearPlane, const float farPlane)
	{
		Matrix4X4 mat;
		mat.Identity();
		mat.m_mat[0][0] = 2.0f / (rightPlane - leftPlane);
		mat.m_mat[1][1] = 2.0f / (bottomPlane - topPlane);
		mat.m_mat[2][2] = 1.0f / (nearPlane - farPlane);
		mat.m_mat[3][2] = -(nearPlane / (farPlane - nearPlane));
		return mat;
	}

	const float** Matrix4X4::GetMatrix()
	{
		return nullptr;
	}

	void Matrix4X4::operator*=(const Matrix4X4& matrix)
	{
		Matrix4X4 out;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				out.m_mat[i][j] =
					m_mat[i][0] * matrix.m_mat[0][j] + m_mat[i][1] * matrix.m_mat[1][j] +
					m_mat[i][2] * matrix.m_mat[2][j] + m_mat[i][3] * matrix.m_mat[3][j];
			}
		}

		SetMatrix(out);
	}

}
