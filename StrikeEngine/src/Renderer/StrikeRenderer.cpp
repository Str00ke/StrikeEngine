#include "Renderer/StrikeRenderer.hpp"
#include <vector>
#include "Core/Time.hpp"
#include "Model/Vertex.hpp"
#include "Model/RenderableResourceController.hpp"
#include "Model/Model.hpp"
#include "Model/Renderable.hpp"
#include "Model/Camera.hpp"
#include "Core/ComPtr.hpp"
#include <d3d12.h>
#include "Tools/D3DUtils.hpp"
#include "Renderer/Device.hpp"

namespace StrikeEngine
{
	StrikeRenderer* StrikeRenderer::m_instance = nullptr;

	StrikeRenderer* StrikeRenderer::Instance()
	{
		if (m_instance == nullptr)
			throw std::runtime_error("Renderer instance is nullptr");

		return m_instance;
	}

	void StrikeRenderer::init()
	{
#if defined(_DEBUG) || defined(DEBUG)
		//Enable D3D12 debug layer
		{
			ComPtr<ID3D12Debug> debugController;
			//Warning: On Windows 10, DirectX Debug layers are listed as optional features (Settings > System > Optional Features > "Graphics Tools"). If not listed, click add feature, then select Graphics Tools.
			D3D::Utils::throwIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
		}
#endif //DEBUG
		
		D3D::Utils::throwIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory5), (void**) & m_dxgiFactory));

		//Try to create hardware device
		m_device = new Device();
	}

	StrikeWindow* StrikeRenderer::GetStrikeWindow()
	{
		return m_strikeWin;
	}

	IDXGIFactory5* StrikeRenderer::getFactory() const
	{
		return m_dxgiFactory;
	}


	StrikeRenderer::StrikeRenderer(StrikeWindow* strikeWindow) :
		m_strikeWin(strikeWindow)
	{
		if (m_instance != nullptr)
			throw std::runtime_error("Renderer already allocated");

		m_instance = this;
	}

	StrikeRenderer::~StrikeRenderer()
	{

		
	}

	
}