#include "Renderer/StrikeRenderer.hpp"
#include <vector>
#include "Vulkan/VkFunctions.hpp"
#include "Core/Time.hpp"
#include "Model/Vertex.hpp"
#include "Model/RenderableResourceController.hpp"
#include "Model/Model.hpp"
#include "Model/Renderable.hpp"
#include "Model/Camera.hpp"


namespace StrikeEngine
{
	StrikeRenderer* StrikeRenderer::m_instance = nullptr;

	StrikeRenderer* StrikeRenderer::Instance()
	{
		if (m_instance == nullptr)
			throw std::runtime_error("Renderer instance is nullptr");

		return m_instance;
	}

	StrikeWindow* StrikeRenderer::GetStrikeWindow()
	{
		return m_strikeWin;
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