#pragma once
#include "Model/UniformBufferObject.hpp"
#include "Math/Matrix4X4.hpp"
#include "Core/StrikeObject.hpp"
#include "Renderer/Resource.hpp"
#include "Renderer/Buffer.hpp"
#include <memory>

namespace StrikeEngine
{
	class Camera
	{
	public:

		Camera();
		void operator=(const Camera&) = delete;
	

		static Camera* Instance();


		void Update(bool _updateUBO = true);

		float GetFOV() const;
		void SetFOV(float newFov);

		float GetFarClip() const;
		void SetFarClip(float newFar);

		float GetNearClip() const;
		void SetNearClip(float newNear);

		void UpdateUbo();

		float computeVerticalFOV();

		float m_rotX = 0.0f;
		float m_rotY = 0.0f;
		float m_forward = 0.0f;
		float m_rightward = 0.0f;
		Matrix4X4 m_worldCam;
		Matrix4X4 m_projCam;
		Matrix4X4 m_viewCam;
		Matrix4X4 m_WorldViewProj;

	private:
		float m_fov = 1.57f;
		float m_verticalFov = 0.0f;
		float m_aspectRatio;
		float m_nearClip = 0.1f;
		float m_farClip = 100.0f;
		std::unique_ptr<UploadBuffer<UniformBufferObject>> m_cameraConstantBuffer = nullptr;
		UniformBufferObject m_cameraUBO;

		void updateProjectionMatrix();
	protected:
		static Camera* m_instance;
	};

}

