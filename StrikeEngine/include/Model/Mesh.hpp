#pragma once
#include <string>
#include "Core/Asset.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include "Model/Vertex.hpp"
#include "Core/defines.hpp"
#include "Core/Blob.hpp"
#include <d3d12.h>
#include <unordered_map>

namespace StrikeEngine
{

	// Defines a subrange of geometry in a MeshGeometry. This is for when
	// multiple geometries are stored in one vertex and index buffer. It
	// provides the offsets and data needed to draw a subset of geometry
	// stores in the vertex and index buffers
	struct SubmeshGeometry
	{
		u32 indexCount = 0;
		u32 startIndexLocation = 0;
		i32 baseVertexLocation = 0;

		// Bounding Box of the geometry defined by this submesh
		// TODO
	};

	class StrikeRenderer;
	class Resource;
	struct MeshGeometry : public Asset
	{
	public:



		// System memory copies. Use Blobs because the vertex/index format can be generic
		Blob vertexBufferCPU = nullptr;
		Blob indexBufferCPU = nullptr;
		
		Resource* vertexBufferGPU = nullptr;
		Resource* indexBufferGPU = nullptr;

		Resource* vertexBufferUploader = nullptr;
		Resource* indexBufferUploader = nullptr;

		// Data about the buffers
		u32 vertexByteStride = 0;
		u32 vertexBufferByteSize = 0;
		DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
		u32 indexBufferByteSize = 0;

		// A MeshGeometry may store multiple geometries in one vertex/index buffer
		// Use this container to define the Submesh geometries so we can draw the Submeshes individually
		std::unordered_map<std::string, SubmeshGeometry> _drawArgs;

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView() const;
		D3D12_INDEX_BUFFER_VIEW indexBufferView() const;

		// We can free this memory after we finish upload to the GPU
		void disposeUploaders();

		Vector3f m_vertexColor = Vector3f(1.0f, 1.0f, 1.0f);
	};


};


