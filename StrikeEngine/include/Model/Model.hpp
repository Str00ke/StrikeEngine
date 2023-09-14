#pragma once
#include "Model/Mesh.hpp"
#include "Model/Texture.hpp"
#include "Model/Renderable.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include "Model/UniformBufferObject.hpp"
#include "Math/Vector3.hpp"


namespace StrikeEngine
{
	

	class Model : public Renderable
	{
	public:
		Model()
		{}

		Mesh& GetMesh();
		Texture& GetTexture();
		Texture& GetDepthTexture();
		BufferParameters& GetUniformBuffer();

		virtual bool Create();

		bool UpdateUniformBuffer();

		float testAngle;
		Vector3f pos{1.0f, 1.0f , 1.0f};

	private:
		Mesh m_mesh;
		Texture m_tex;
		Texture m_depth;

		BufferParameters UniformBuffer;
		void* UniformBufferMapped;

		bool CreateUniformBuffer();
	};

};



