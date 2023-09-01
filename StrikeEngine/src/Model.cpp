#include "Model.hpp"
#include "Time.hpp"
#include "Camera.hpp"

namespace StrikeEngine
{
	Mesh& Model::GetMesh()
	{
		return m_mesh;
	}

	Texture& Model::GetTexture()
	{
		return m_tex;
	}

	bool Model::Create()
	{
		if (!CreateUniformBuffer())
			return false;

		//Depth Texture
		if (!m_depth.CreateDepthTexture(StrikeRenderer::Instance()))
			return false;
		//--------------------------------------------------------------------------

		if (!m_tex.Create(StrikeRenderer::Instance(), this))
			return false;

		if (!m_mesh.Create(StrikeRenderer::Instance()))
			return false;

		return true;
	}

	bool Model::UpdateUniformBuffer()
	{
		float time = Time::GetCurrTime();

		UniformBufferObject ubo{};
		/*ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), StrikeRenderer::Instance()->GetVulkanParameters().SwapChain.Extent.width / (float)StrikeRenderer::Instance()->GetVulkanParameters().SwapChain.Extent.height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;*/

		ubo.model.Identity();
		ubo.model.m_mat[3][0] = pos.x;
		ubo.model.m_mat[3][1] = pos.y;
		ubo.model.m_mat[3][2] = pos.z;
		ubo.view = Camera::Instance()->m_viewCam;
		ubo.proj = Camera::Instance()->m_projCam;
		auto camVec = Camera::Instance()->m_worldCam.GetTranslation();
		Vector4f vec{ camVec.x, camVec.y, camVec.z, 1.0f };
		ubo.camPos = vec;

		memcpy(UniformBufferMapped, &ubo, sizeof(ubo));
		return true;
	}

	bool Model::CreateUniformBuffer()
	{
		UniformBuffer.Size = sizeof(UniformBufferObject);
		/*if (!CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Vulkan.UniformBuffer))
		{
			std::cout << "Could not create uniform buffer" << std::endl;
			return false;
		}*/
		if (!StrikeRenderer::Instance()->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBuffer))
		{
			std::cout << "Could not create uniform buffer" << std::endl;
			return false;
		}

		void* stagingBufferMemoryPointer;
		if (vkMapMemory(StrikeRenderer::Instance()->GetVulkanParameters().Device, UniformBuffer.Memory, 0, UniformBuffer.Size, 0, &UniformBufferMapped) != VK_SUCCESS)
		{
			std::cout << "Could not map memory and upload data to a staging buffer" << std::endl;
			return false;
		}

		return true;
	}

	Texture& Model::GetDepthTexture()
	{
		return m_depth;
	}

	BufferParameters& Model::GetUniformBuffer()
	{
		return UniformBuffer;
	}

}

