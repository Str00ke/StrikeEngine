#pragma once
#include "Model/UniformBufferObject.hpp"
#include "Math/Matrix4X4.hpp"

namespace StrikeEngine
{
	class Camera
	{
	public:

		Camera();
		void operator=(const Camera&) = delete;
	

		static Camera* Instance();


		void Update();

		float GetFOV() const;
		void SetFOV(float newFov);

		float GetFarClip() const;
		void SetFarClip(float newFar);

		float GetNearClip() const;
		void SetNearClip(float newNear);

		float m_rotX = 0.0f;
		float m_rotY = 0.0f;
		float m_forward = 0.0f;
		float m_rightward = 0.0f;
		Matrix4X4 m_worldCam;
		Matrix4X4 m_projCam;
		Matrix4X4 m_viewCam;

	private:
		float m_fov = 1.57f;
		float m_nearClip = 0.1f;
		float m_farClip = 100.0f;

	protected:
		static Camera* m_instance;
	};

}

