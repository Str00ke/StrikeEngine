#include "Renderer/Buffer.hpp"
#include "Renderer/Resource.hpp"
#include "Renderer/Device.hpp"
#include "Renderer/CommandList.hpp"
//#include "d3dx12.h"
#include "d3dx12_resource_helpers_CUSTOM.h"
#include <iostream>


using namespace StrikeEngine;

const Buffer* Buffer::CreateVertexBuffer()
{
    Buffer* buf = new Buffer(BufferType::VERTEX);
	// TODO
	return buf;
}

const Buffer* Buffer::CreateIndexBuffer()
{
	Buffer* buf = new Buffer(BufferType::INDEX);
	// TODO
	return buf;
}

const Buffer* StrikeEngine::Buffer::CreateDefaultBuffer(Device* _device, CommandList* _graphicsCommandList, const void* _initData, u64 _byteSize, Resource* _uploadBuffer)
{
	_riid = _device->getRIID(); // Since the UpdateSubresources function in d3dx12_resource_helpers file call a GetDevice with a hardcoded RIID (IID_ID3D12Device), had to modify the file to give a custom RIID. Crap fix, I know...

	// Create the actual default buffer resource
	Resource* defaultBuffer = Resource::Buffer(_device, _byteSize);

	// In order to copy CPU memory data into out default buffer, we need to create an intermediate upload heap.
	_uploadBuffer = Resource::Buffer(_device, D3D12_HEAP_TYPE_UPLOAD, D3D12_HEAP_FLAG_NONE, _byteSize, D3D12_RESOURCE_STATE_GENERIC_READ);

	// Describe the data we want to copy into the default buffer
	D3D12_SUBRESOURCE_DATA subResData = {};
	subResData.pData = _initData;
	subResData.RowPitch = _byteSize;
	subResData.SlicePitch = subResData.RowPitch;

	// Schedule to copy the data to the default buffer resource.
	// At a high level, the helper function UpdateSubresources
	// will copy the CPU memory into the intermediate upload heap.
	// Then, using ID3D12CommandList::CopySubresourceRegion,
	// the intermediate upload heap data will be copied to mBuffer.
	_graphicsCommandList->resourceBarrier(1, _graphicsCommandList->transition(defaultBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources<1>(_graphicsCommandList->getCommandListAPI(), defaultBuffer->getResourceAPI(), _uploadBuffer->getResourceAPI(), 0, 0, 1, &subResData);
	_graphicsCommandList->resourceBarrier(1, _graphicsCommandList->transition(defaultBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	// Note: uploadBuffer has to be kept alive after the above function
	// calls because the command list has not been executed yet that
	// performs the actual copy.
	// The caller can Release the uploadBuffer after it knows the copy
	// has been executed.
	return new Buffer(defaultBuffer);
}
