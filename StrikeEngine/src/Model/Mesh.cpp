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

#include "Renderer/Resource.hpp"

namespace std
{
#if VK
	template<> struct hash<StrikeEngine::Vertex>
	{
		size_t operator()(StrikeEngine::Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) << 1) ^
				(hash<glm::vec3>()(vertex.color) << 1);
		};
	};
#endif // VK
}

namespace StrikeEngine
{
	D3D12_VERTEX_BUFFER_VIEW MeshGeometry::vertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = vertexBufferGPU->getResourceAPI()->GetGPUVirtualAddress();
		vbv.StrideInBytes = vertexByteStride;
		vbv.SizeInBytes = vertexBufferByteSize;

		return vbv;
	}


	D3D12_INDEX_BUFFER_VIEW MeshGeometry::indexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = indexBufferGPU->getResourceAPI()->GetGPUVirtualAddress();
		ibv.Format = indexFormat;
		ibv.SizeInBytes = indexBufferByteSize;

		return ibv;
	}

	void MeshGeometry::disposeUploaders()
	{
		vertexBufferUploader = nullptr;
		indexBufferUploader = nullptr;
	}
}

