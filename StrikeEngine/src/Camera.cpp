#include "Camera.hpp"
#include "Matrix4X4.hpp"
#include "StrikeRenderer.hpp"

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

		auto& vk = StrikeRenderer::Instance()->GetVulkanParameters();
		m_projCam.SetPerspectiveProjectionMatrix(vk.SwapChain.Extent.width / (float)vk.SwapChain.Extent.height, 1.57f, 0.1f, 100.0f);
	}

}
