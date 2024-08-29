#pragma once
#include <d3d12.h>
#include "Core/ComPtr.hpp"
#include <dxgi1_5.h>

namespace StrikeEngine
{
	class DescriptorHeap;
	class Device
	{
	public:
		Device();
		Device(const Device&) = default;
		Device(Device&&) = delete;
		Device& operator=(const Device&) = delete;
		Device& operator=(Device&&) = delete;
		~Device() = default;


		void DX12_checkFeatureSupport(D3D12_FEATURE _feature, void* _featureSupportData, UINT _featureSupportDataSize);
		ID3D12Device* getDeviceAPI() const;

		const UINT getNodeCount() const;

		void createDevice();
		IDXGIFactory5* getFactory() const;

		IID getRIID();

		void enableDebugValidationLayer();

	private:
		ComPtr<ID3D12Device> m_device;
		IDXGIFactory5* m_dxgiFactory;

	};
}


