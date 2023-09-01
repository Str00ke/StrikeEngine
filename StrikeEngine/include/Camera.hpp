#pragma once
#include "UniformBufferObject.hpp"
#include "Matrix4X4.hpp"

namespace StrikeEngine
{
	class Camera
	{
	public:

		Camera();
		void operator=(const Camera&) = delete;
	

		static Camera* Instance();


		void Update();

		float m_rotX = 0.0f;
		float m_rotY = 0.0f;
		float m_forward = 0.0f;
		float m_rightward = 0.0f;
		Matrix4X4 m_worldCam;
		Matrix4X4 m_projCam;
		Matrix4X4 m_viewCam;

	protected:
		static Camera* m_instance;
	};

}

