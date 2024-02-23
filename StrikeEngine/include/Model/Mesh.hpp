#pragma once
#include <string>
#include "Core/Asset.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include "Model/Vertex.hpp"

namespace StrikeEngine
{

	class StrikeRenderer;
	class Mesh : public Asset
	{
	public:
		bool Create(StrikeRenderer* renderer);
		/*MeshBuffer<Vertex> GetVertexBuffer();
		MeshBuffer<uint32_t> GetIndexBuffer();*/
		void SetVertexColor(const Vector3f& color);
		void SetVertexBuffer(const std::vector<Vertex>& vertices);
		void SetIndexBuffer(const std::vector<uint32_t>& indices);


	private:
		bool LoadOBJFile();
		bool CreateVertexBuffer(StrikeRenderer* renderer);
		bool CreateIndexBuffer(StrikeRenderer* renderer);

		/*MeshBuffer<Vertex> m_vertexBuffer;
		MeshBuffer<uint32_t> m_indicesBuffer;*/
		Vector3f m_vertexColor = Vector3f(1.0f, 1.0f, 1.0f);

		StrikeRenderer* m_renderer;

		/*std::vector<VertexData> m_vertices;
		BufferParameters m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;

		std::vector<uint32_t> m_indices;
		BufferParameters m_indexBuffer;
		VkDeviceMemory m_indexBufferMemory;*/

	};

};


