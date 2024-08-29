#pragma once
#include "Core/defines.hpp"
#include "Core/ComPtr.hpp"
#include <d3d12.h>
namespace StrikeEngine
{
	class Device;
	class Resource
	{
	public:
		Resource() = default;
		Resource(Device* _device, D3D12_RESOURCE_DESC _desc);
		Resource(Device* _device, const D3D12_HEAP_TYPE& _heapTypes, const D3D12_HEAP_FLAGS& _heapFlags, const D3D12_RESOURCE_DESC& _desc, const D3D12_RESOURCE_STATES& _resStates, const D3D12_CLEAR_VALUE& _clearValue);
		Resource(const Resource&) = default;
		Resource(Resource&&) = delete;
		Resource& operator=(const Resource&) = delete;
		Resource& operator=(Resource&&) = delete;
		~Resource() = default;

		ID3D12Resource* getResourceAPI() const;
		ComPtr<ID3D12Resource> getResourceComPtr() const;
		static Resource* Buffer(Device* _device, u64 _width);
		static Resource* Buffer(Device* _device, const D3D12_HEAP_TYPE& _heapTypes, const D3D12_HEAP_FLAGS& _heapFlags, const u64 _width, const D3D12_RESOURCE_STATES& _resStates, D3D12_CLEAR_VALUE* _clearValue = nullptr);

	private:
		ComPtr<ID3D12Resource> m_resource;
	};

}


