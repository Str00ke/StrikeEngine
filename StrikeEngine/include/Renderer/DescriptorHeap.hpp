#pragma once
#include "Core/defines.hpp"
#include <d3d12.h>
#include "Core/ComPtr.hpp"

namespace StrikeEngine
{
	class Device;
	class DescriptorHeap
	{
	public:
		DescriptorHeap() = default;
		DescriptorHeap(const DescriptorHeap&) = default;
		DescriptorHeap(DescriptorHeap&&) = delete;
		DescriptorHeap& operator=(const DescriptorHeap&) = delete;
		DescriptorHeap& operator=(DescriptorHeap&&) = delete;
		~DescriptorHeap() = default;

		ID3D12DescriptorHeap* getDescriptorHeapAPI() const;
		ComPtr<ID3D12DescriptorHeap> getDescriptorHeapCom() const;

		void create(const D3D12_DESCRIPTOR_HEAP_DESC& _desc, Device* _device);

	private:
		ComPtr<ID3D12DescriptorHeap> m_descHeap;
	};
}


