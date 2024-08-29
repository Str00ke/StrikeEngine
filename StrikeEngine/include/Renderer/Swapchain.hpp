#pragma once
#include "Core/defines.hpp"
#include <dxgi.h>
#include "Core/ComPtr.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include "Renderer/Resource.hpp"

namespace StrikeEngine
{
	class Swapchain
	{
	public:
		Swapchain();
		Swapchain(const Swapchain&) = default;
		Swapchain(Swapchain&&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;
		Swapchain& operator=(Swapchain&&) = delete;
		~Swapchain() = default;

		void createSwapchain(StrikeRenderer& _rend);
		IDXGISwapChain* getSwapchainAPI();

		void resizeBuffers(); // TODO

		void present(u32 _syncInterval, u32 _flags);

		Resource* getBuffer(u32 _bufferIndex);
		ComPtr<ID3D12Resource> m_swapchainBuffer[StrikeRenderer::swapchainBufferCount];
	private:
		ComPtr<IDXGISwapChain> m_swapchain;


	};
}


