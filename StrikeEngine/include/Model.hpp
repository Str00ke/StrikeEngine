#pragma once
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Renderable.hpp"
#include "StrikeRenderer.hpp"
#include "UniformBufferObject.hpp"
#include "Vector3.hpp"


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



