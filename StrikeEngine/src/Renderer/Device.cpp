#include "Renderer/Device.hpp"
#include "Tools/D3DUtils.hpp"
#include "Core/ComPtr.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include <dxgi1_4.h>

StrikeEngine::Device::Device()
{
	createDevice();
}

void StrikeEngine::Device::DX12_checkFeatureSupport(D3D12_FEATURE _feature, void* _featureSupportData, UINT _featureSupportDataSize)
{
	m_device->CheckFeatureSupport(_feature, _featureSupportData, _featureSupportDataSize);
}

ID3D12Device* StrikeEngine::Device::getDeviceAPI() const
{
	return m_device.Get();
}

const UINT StrikeEngine::Device::getNodeCount() const
{
	return m_device.Get()->GetNodeCount();
}

void StrikeEngine::Device::createDevice()
{
	HRESULT hwResult = D3D12CreateDevice(nullptr/*=> Default adapter*/, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
	
	//Fallback to WARP device
	if (FAILED(hwResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		D3D::Utils::throwIfFailed(StrikeRenderer::Instance()->getFactory()->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		D3D::Utils::throwIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)));
	}
}
