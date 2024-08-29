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

#if defined(_DEBUG) || defined(DEBUG)
	//Enable D3D12 debug layer
	{
		ComPtr<ID3D12Debug> debugController;
		//Warning: On Windows 10, DirectX Debug layers are listed as optional features (Settings > System > Optional Features > "Graphics Tools"). If not listed, click add feature, then select Graphics Tools.
		D3D::Utils::throwIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}

	// Enable GBV (GPU Based Validation)
	enableDebugValidationLayer();
#endif //DEBUG

	D3D::Utils::throwIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory5), (void**)&m_dxgiFactory));

	HRESULT hwResult = D3D12CreateDevice(nullptr/*=> Default adapter*/, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
	
	//D3D12CreateDevice failed, fallback to WARP device
	if (FAILED(hwResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		D3D::Utils::throwIfFailed(StrikeRenderer::Instance()->getDevice()->getFactory()->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		D3D::Utils::throwIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)));
	}
}

IDXGIFactory5* StrikeEngine::Device::getFactory() const
{
	return m_dxgiFactory;
}

IID StrikeEngine::Device::getRIID()
{
	return __uuidof(**(&m_device));
}

void StrikeEngine::Device::enableDebugValidationLayer()
{
	ID3D12Debug* debugInterface;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
	{
		debugInterface->EnableDebugLayer();
		debugInterface->Release();

		//if (useGPUBasedValidation)
		{
			ID3D12Debug1* debugInterface1;
			if (SUCCEEDED((debugInterface->QueryInterface(IID_PPV_ARGS(&debugInterface1)))))
			{
				debugInterface1->SetEnableGPUBasedValidation(true);
				debugInterface1->Release();
			}
			else
			{
				std::cout << "GBV failed initializing" << std::endl;
			}
		}
	}
	else
	{
		std::cout << "GBV failed initializing" << std::endl;
	}
}


