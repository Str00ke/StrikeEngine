#include "Renderer/Swapchain.hpp"
#include "Tools/D3DUtils.hpp"
#include "Renderer/CommandQueue.hpp"
#include "Renderer/DescriptorHeap.hpp"

StrikeEngine::Swapchain::Swapchain()
{
	/*for (Resource* rsc : m_swapchainBuffer)
	{
		rsc = new Resource();
	}*/
}

void StrikeEngine::Swapchain::createSwapchain(StrikeRenderer& _rend)
{
	m_swapchain.Reset();
	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	swapchainDesc.BufferDesc.Width = _rend.GetStrikeWindow()->GetParams().Width;
	swapchainDesc.BufferDesc.Height = _rend.GetStrikeWindow()->GetParams().Height;
	swapchainDesc.BufferDesc.RefreshRate.Numerator = 60; // Warning : Hardcoded value
	swapchainDesc.BufferDesc.RefreshRate.Denominator = 1; // Warning : Hardcoded value
	swapchainDesc.BufferDesc.Format = _rend.getBackBufferFormat();
	swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapchainDesc.SampleDesc.Count = 1; // Warning : Hardcoded value, checkMSAAQualitySupport return D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS with both Count and quality filled
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.BufferCount = _rend.swapchainBufferCount;
	swapchainDesc.OutputWindow = _rend.GetStrikeWindow()->GetParams().Handle;
	swapchainDesc.Windowed = true; // Warning : Hardcoded value
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// Note : Swapchain uses queue to perform flush
	D3D::Utils::throwIfFailed(_rend.getDevice()->getFactory()->CreateSwapChain(_rend.cmdQueue->getCommandQueueAPI(), &swapchainDesc, m_swapchain.GetAddressOf()));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(_rend.m_rtvHeap->getDescriptorHeapAPI()->GetCPUDescriptorHandleForHeapStart());
	for (u32 i = 0; i < StrikeRenderer::swapchainBufferCount; ++i)
	{
		// Get the ith buffer in the swapchain
		D3D::Utils::throwIfFailed(m_swapchain.Get()->GetBuffer(i, IID_PPV_ARGS(&m_swapchainBuffer[i])));

		// Create a render target view to it
		_rend.getDevice()->getDeviceAPI()->CreateRenderTargetView(m_swapchainBuffer[i].Get(), nullptr, rtvHeapHandle);

		// Next entry in heap
		rtvHeapHandle.Offset(1, _rend.m_rtvDescriptorSize);
	}
	
}

IDXGISwapChain* StrikeEngine::Swapchain::getSwapchainAPI()
{
	return m_swapchain.Get();
}

void StrikeEngine::Swapchain::present(u32 _syncInterval, u32 _flags)
{
	m_swapchain.Get()->Present(_syncInterval, _flags);
}

StrikeEngine::Resource* StrikeEngine::Swapchain::getBuffer(u32 _bufferIndex)
{
	/*auto comPtr = m_swapchainBuffer[_bufferIndex]->getResourceComPtr();
	D3D::Utils::throwIfFailed(m_swapchain->GetBuffer(_bufferIndex, IID_PPV_ARGS(&comPtr)));
	return m_swapchainBuffer[_bufferIndex];*/
	return nullptr;
}
