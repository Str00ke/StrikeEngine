#pragma once
#include "Model/Mesh.hpp"
#include "Model/Texture.hpp"
#include "Model/Renderable.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include "Model/UniformBufferObject.hpp"
#include "Math/Vector3.hpp"
#include "Core/StrikeObject.hpp"
#include "Math/Transform.hpp"

namespace StrikeEngine
{
	

	class Model : public Renderable, StrikeObject
	{
	public:
		Model()
		{}

		Mesh& GetMesh();
		void SetMesh(const Mesh& mesh);
		Texture& GetTexture();
		Texture& GetDepthTexture();
		//BufferParameters& GetUniformBuffer();

		bool Create();

		bool UpdateUniformBuffer();

		Transform transform;

	private:
		Mesh m_mesh;
		Texture m_tex;
		Texture m_depth;

		//BufferParameters UniformBuffer;
		void* UniformBufferMapped;

		bool CreateUniformBuffer();
	};

};



