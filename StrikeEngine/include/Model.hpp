#pragma once
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Renderable.hpp"
#include "StrikeRenderer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/hash.hpp"

namespace StrikeEngine
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

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

	private:
		Mesh m_mesh;
		Texture m_tex;
		Texture m_depth;

		BufferParameters UniformBuffer;
		void* UniformBufferMapped;

		bool CreateUniformBuffer();
	};

};



