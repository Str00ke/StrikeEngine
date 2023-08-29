#pragma once
#include <string>
#include "Asset.hpp"
#include "StrikeRenderer.hpp"
#include "Vertex.hpp"

namespace StrikeEngine
{
	template<typename T>
	struct MeshBuffer
	{
		std::vector<T> m_Buffer;
		BufferParameters m_Handle;
		VkDeviceMemory m_BufferMemory;

		MeshBuffer() :
			m_Buffer(),
			m_Handle(),
			m_BufferMemory(VK_NULL_HANDLE)
		{}
	};

	class StrikeRenderer;
	class Mesh : public Asset
	{
	public:
		bool Create(StrikeRenderer* renderer);
		MeshBuffer<Vertex> GetVertexBuffer();
		MeshBuffer<uint32_t> GetIndexBuffer();

	private:
		bool LoadOBJFile();
		bool CreateVertexBuffer(StrikeRenderer* renderer);
		bool CreateIndexBuffer(StrikeRenderer* renderer);

		MeshBuffer<Vertex> m_vertexBuffer;
		MeshBuffer<uint32_t> m_indicesBuffer;

		/*std::vector<VertexData> m_vertices;
		BufferParameters m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;

		std::vector<uint32_t> m_indices;
		BufferParameters m_indexBuffer;
		VkDeviceMemory m_indexBufferMemory;*/
	};

};


