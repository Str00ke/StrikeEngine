#include "Model/Mesh.hpp"
#include "Renderer/StrikeRenderer.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include "../Externals/tiny_obj_loader.h"
#include "Model/Vertex.hpp"
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/hash.hpp"

namespace std
{
	template<> struct hash<StrikeEngine::Vertex>
	{
		size_t operator()(StrikeEngine::Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) << 1) ^
				(hash<glm::vec3>()(vertex.color) << 1);
		};
	};
}

namespace StrikeEngine
{
	bool Mesh::Create(StrikeRenderer* renderer)
	{
		m_renderer = renderer;

		if (!LoadOBJFile())
			return false;

		if (!CreateVertexBuffer(m_renderer))
			return false;

		if (!CreateIndexBuffer(m_renderer))
			return false;
	}

	MeshBuffer<StrikeEngine::Vertex> StrikeEngine::Mesh::GetVertexBuffer()
	{
		return m_vertexBuffer;
	}

	MeshBuffer<uint32_t> StrikeEngine::Mesh::GetIndexBuffer()
	{
		return m_indicesBuffer;
	}

	void Mesh::SetVertexColor(const Vector3f& color)
	{
		m_vertexColor = color;
	}

	void Mesh::SetIndexBuffer(const std::vector<uint32_t>& indices)
	{
		m_indicesBuffer.m_Buffer = indices;
	}

	void Mesh::SetVertexBuffer(const std::vector<Vertex>& vertices)
	{
		m_vertexBuffer.m_Buffer = vertices;
	}

	bool Mesh::LoadOBJFile()
	{
		if (GetPath() == "") //TODO: when create primitive types, no path set, but throw when no path but no primitive
			return true;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, GetPath().c_str()))
			throw std::runtime_error(warn + err);

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vData;
				vData.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vData.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vData.color = { m_vertexColor.x, m_vertexColor.y, m_vertexColor.z };
				m_vertexBuffer.m_Buffer.push_back(vData);
				if (uniqueVertices.count(vData) == 0)
				{
					uniqueVertices[vData] = static_cast<uint32_t>(m_vertexBuffer.m_Buffer.size());
					m_vertexBuffer.m_Buffer.push_back(vData);
				}

				m_indicesBuffer.m_Buffer.push_back(uniqueVertices[vData]);
			}
		}

		return true;
	}
	bool Mesh::CreateVertexBuffer(StrikeRenderer* renderer)
	{
		VkDeviceSize bufferSize = sizeof(m_vertexBuffer.m_Buffer[0]) * m_vertexBuffer.m_Buffer.size();

		m_vertexBuffer.m_Handle.Size = static_cast<uint32_t>(sizeof(m_vertexBuffer.m_Buffer[0]) * m_vertexBuffer.m_Buffer.size());
		renderer->GetVulkanParameters().StagingBuffer.Size = static_cast<uint32_t>(sizeof(m_vertexBuffer.m_Buffer[0]) * m_vertexBuffer.m_Buffer.size());

		if (!renderer->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, renderer->GetVulkanParameters().StagingBuffer))
		{
			std::cout << "Could not create vertex buffer" << std::endl;
			return false;
		}

		void* data;
		vkMapMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory, 0, bufferSize, 0, &data);
		memcpy(data, m_vertexBuffer.m_Buffer.data(), (size_t)bufferSize);
		vkUnmapMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory);

		if (!renderer->CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer.m_Handle))
		{
			std::cout << "Could not create vertex buffer" << std::endl;
			return false;
		}


		//copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
		VkCommandBuffer commandBuffer = renderer->BeginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.size = bufferSize;
		vkCmdCopyBuffer(commandBuffer, renderer->GetVulkanParameters().StagingBuffer.Handle, m_vertexBuffer.m_Handle.Handle, 1, &copyRegion);

		renderer->EndSingleTimeCommands(commandBuffer);

		vkDestroyBuffer(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Handle, nullptr);
		vkFreeMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory, nullptr);

		return true;
	}
	bool Mesh::CreateIndexBuffer(StrikeRenderer* renderer)
	{
		VkDeviceSize bufferSize = sizeof(m_indicesBuffer.m_Buffer[0]) * m_indicesBuffer.m_Buffer.size();

		m_indicesBuffer.m_Handle.Size = static_cast<uint32_t>(sizeof(m_indicesBuffer.m_Buffer[0]) * m_indicesBuffer.m_Buffer.size());
		renderer->GetVulkanParameters().StagingBuffer.Size = static_cast<uint32_t>(sizeof(m_indicesBuffer.m_Buffer[0]) * m_indicesBuffer.m_Buffer.size());

		if (!renderer->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, renderer->GetVulkanParameters().StagingBuffer))
		{
			std::cout << "Could not create vertex buffer" << std::endl;
			return false;
		}

		void* data;
		vkMapMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory, 0, bufferSize, 0, &data);
		memcpy(data, m_indicesBuffer.m_Buffer.data(), (size_t)bufferSize);
		vkUnmapMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory);

		if (!renderer->CreateBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indicesBuffer.m_Handle))
		{
			std::cout << "Could not create vertex buffer" << std::endl;
			return false;
		}
		//if (!CopyVertexData())
		//	return false;


		//copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
		VkCommandBuffer commandBuffer = renderer->BeginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.size = bufferSize;
		vkCmdCopyBuffer(commandBuffer, renderer->GetVulkanParameters().StagingBuffer.Handle, m_indicesBuffer.m_Handle.Handle, 1, &copyRegion);

		renderer->EndSingleTimeCommands(commandBuffer);

		vkDestroyBuffer(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Handle, nullptr);
		vkFreeMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory, nullptr);

		return true;
	}

}

