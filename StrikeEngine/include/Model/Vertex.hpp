#pragma once
#include "d3d12.h"
#include "Math/Vector3.hpp"

namespace StrikeEngine
{
	struct Vertex
	{
#if VK
#include "glm/glm.hpp"
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		bool operator==(const Vertex& other) const
		{
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
#else

		Vector3f pos;
		Vector3f color;
		//Vector3f texCoord;

		bool operator==(const Vertex& other) const
		{
			return (pos == other.pos) && (color == other.color)/* && (texCoord == other.texCoord)*/;
		}

		// If the structure change in any way (add/remove/modify members), do not forget to modify this function too, accordingly to the update
		//static const D3D12_INPUT_ELEMENT_DESC* getVertexStructureDescriptor()
		//{
		//	D3D12_INPUT_ELEMENT_DESC desc[] =
		//	{
		//		{
		//			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		//		},
		//		{
		//			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12 /* 3 * 4 bytes (float) */, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		//		},
		//		{
		//			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24 /* (3 * 4) * 2 bytes (float) */, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		//		}
		//	};

		//	return desc;
		//}

		static const std::vector<std::vector<D3D12_INPUT_ELEMENT_DESC>> getVertexStructureDescriptor()
		{

			std::vector<std::vector<D3D12_INPUT_ELEMENT_DESC>> ret = {};
			std::vector<D3D12_INPUT_ELEMENT_DESC> desc =
			{
				{
					"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
				},
				{
					"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 /* 3 * 4 bytes (float) */, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
				}
				//,
				//{
				//	"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24 /* (3 * 4) * 2 bytes (float) */, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
				//}
			};

			ret.push_back(desc);

			return ret;
		}

#endif // VK


	};
}



