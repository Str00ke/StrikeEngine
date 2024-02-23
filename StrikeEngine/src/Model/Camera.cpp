#include "Model/Camera.hpp"
#include "Math/Matrix4X4.hpp"
#include "Renderer/StrikeRenderer.hpp"
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
	}

	Camera* Camera::Instance()
	{
		if (m_instance == nullptr)
			throw std::runtime_error("Camera instance is nullptr");

		return m_instance;
	}

	void Camera::Update()
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

		//auto& vk = StrikeRenderer::Instance()->GetVulkanParameters();
		//m_projCam.SetPerspectiveProjectionMatrix(vk.SwapChain.Extent.width / (float)vk.SwapChain.Extent.height, m_fov, m_nearClip, m_farClip);
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

}
