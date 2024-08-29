#include "Renderer/Resource.hpp"
#include <iostream>
#include "Tools/D3DUtils.hpp"
#include "Renderer/Device.hpp"
#include "../Externals/d3dx12.h"

StrikeEngine::Resource::Resource(Device* _device, D3D12_RESOURCE_DESC _desc)
{
	D3D::Utils::throwIfFailed(_device->getDeviceAPI()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&_desc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&m_resource)));
}

StrikeEngine::Resource::Resource(Device* _device, const D3D12_HEAP_TYPE& _heapTypes, const D3D12_HEAP_FLAGS& _heapFlags, const D3D12_RESOURCE_DESC& _desc, const D3D12_RESOURCE_STATES& _resStates, const D3D12_CLEAR_VALUE& _clearValue)
{
	D3D::Utils::throwIfFailed(_device->getDeviceAPI()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(_heapTypes),
		_heapFlags,
		&_desc,
		_resStates,
		&_clearValue,
		IID_PPV_ARGS(&m_resource)));
}

ID3D12Resource* StrikeEngine::Resource::getResourceAPI() const
{
    return m_resource.Get();
}

ComPtr<ID3D12Resource> StrikeEngine::Resource::getResourceComPtr() const
{
    return m_resource;
}

StrikeEngine::Resource* StrikeEngine::Resource::Buffer(Device* _device, u64 _width)
{
	return new StrikeEngine::Resource(_device, CD3DX12_RESOURCE_DESC::Buffer(_width));
}

StrikeEngine::Resource* StrikeEngine::Resource::Buffer(Device* _device, const D3D12_HEAP_TYPE& _heapTypes, const D3D12_HEAP_FLAGS& _heapFlags, const u64 _width, const D3D12_RESOURCE_STATES& _resStates, D3D12_CLEAR_VALUE* _clearValue)
{
	return new StrikeEngine::Resource(_device, _heapTypes, _heapFlags, CD3DX12_RESOURCE_DESC::Buffer(_width), _resStates, *_clearValue);
}
