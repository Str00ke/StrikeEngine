#pragma once

class Matrix4X4
{
public:
	Matrix4X4();
	Matrix4X4(const Matrix4X4&) = delete;
	Matrix4X4(Matrix4X4&&) = delete;
	~Matrix4X4()
	{};
	void Identity();
	void SetPosition();
	void SetRotation();
	void SetScale();

	static Matrix4X4& GetPerspectiveProjectionMatrix(const float aspectRatio, const float fieldOfView, const float nearClip, const float farClip);
	static Matrix4X4& GetOrthographicProjectionMatrix(const float leftPlane, const float rightPlane, const float topPlane, const float bottomPlane, const float nearPlane, const float farPlane);

	const float** GetMatrix();

public:
	float m_mat[4][4] = {};
};

