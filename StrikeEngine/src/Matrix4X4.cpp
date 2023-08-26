#include "Matrix4X4.hpp"
#include <cmath>


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
	m_mat[0][0] = 1.0f;
	m_mat[1][1] = 1.0f;
	m_mat[2][2] = 1.0f;
	m_mat[3][3] = 1.0f;
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
