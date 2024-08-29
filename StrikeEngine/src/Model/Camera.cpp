#include "Model/Camera.hpp"
#include "Math/Matrix4X4.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include "Tools/D3DUtils.hpp"
#include "Math/Math.hpp"

namespace StrikeEngine
{
	Camera* Camera::m_instance = nullptr;


	Camera::Camera()
	{
		if (m_instance != nullptr)
			throw std::runtime_error("Camera already allocated");

		m_instance = this;

		m_worldCam.Identity();
		m_viewCam.Identity();
		m_projCam.Identity();

		//m_cameraConstantBuffer = std::make_unique<UploadBuffer<UniformBufferObject>>(StrikeRenderer::Instance()->getDevice(), 1, true);
	}

	Camera* Camera::Instance()
	{
		if (m_instance == nullptr)
			throw std::runtime_error("Camera instance is nullptr");

		return m_instance;
	}

	void Camera::Update(bool _updateUBO)
	{
		Matrix4X4 worldCam, tmp;
		worldCam.Identity();
		tmp.Identity();
		tmp.SetRotationX(m_rotX);
		worldCam *= tmp;

		tmp.Identity();
		tmp.SetRotationY(m_rotY);
		worldCam *= tmp;

		Vector3f newPos = m_worldCam.GetTranslation() + worldCam.GetZDirection() * (m_forward * 0.05f);

		newPos = newPos + worldCam.GetXDirection() * (m_rightward * 0.05f);

		worldCam.SetTranslation(newPos);

		m_worldCam = worldCam;
		worldCam.Inverse();
		m_viewCam = worldCam;
		updateProjectionMatrix();

		Matrix4X4 m1 = m_worldCam;
		Matrix4X4 m2 = m1 * m_viewCam;
		Matrix4X4 m3 = m2 * m_projCam;

		m_WorldViewProj = m3;
	}

	float Camera::GetFOV() const
	{
		return m_fov;
	}

	void Camera::SetFOV(float newFov)
	{
		//m_fov = Math::Max<float>(0.1f, newFov); //FIXME: Unresolved external
		m_fov = newFov > 0.1f ? newFov : 0.1f;
	}

	float Camera::GetFarClip() const
	{
		return m_farClip;
	}

	void Camera::SetFarClip(float newFar)
	{
		m_farClip = newFar > 0.1f ? newFar : 0.1f;
	}

	float Camera::GetNearClip() const
	{
		return m_nearClip;
	}

	void Camera::SetNearClip(float newNear)
	{
		m_nearClip = newNear > 0.1f ? newNear : 0.1f;
	}

	void Camera::UpdateUbo()
	{
		m_cameraConstantBuffer->copyData(0, m_cameraUBO);
	}

	float Camera::computeVerticalFOV()
	{
		//VFOV = 2*atan*(tan(h/2)*AR) => h = horizontal FOV | AR = aspectRatio
		m_aspectRatio = StrikeRenderer::Instance()->getAspectRatio();
		float& hFov = m_fov;

		m_verticalFov = 2 * atan(tan(hFov / 2) * m_aspectRatio);
		return m_verticalFov;
	}

	void Camera::updateProjectionMatrix()
	{
		computeVerticalFOV();
		// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Camera.cpp
		float y = 1.0f / std::tanf(m_verticalFov * 0.5f);
		float x = y * m_aspectRatio;

		float q1, q2;
		q1 = m_farClip / (m_nearClip - m_farClip);
		q2 = q1 * m_nearClip;

		m_projCam.Identity();

		Vector4f r1(x, 0.0f, 0.0f, 0.0f);
		Vector4f r2(0.0f, y, 0.0f, 0.0f);
		Vector4f r3(0.0f, 0.0f, q1, -1.0f);
		Vector4f r4(0.0f, 0.0f, q2, 0.0f);

		m_projCam.SetRaw(r1, r2, r3, r4);


		//Matrix4::SetPerspectiveProjectionMatrix()
	}

}
