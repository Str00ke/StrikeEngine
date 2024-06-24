#include "Core/StrikeApp.hpp"
#include "Model/RenderableResourceController.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include "Core/OS.hpp"

namespace StrikeEngine
{

	StrikeApp* StrikeApp::m_Instance = nullptr;;

	StrikeApp::StrikeApp()
	{
		m_isRunning = true;
		m_Instance = this;
		//resourceController = new RenderableResourceController();
	}

	void StrikeApp::init(StrikeWindow* _window)
	{
		m_renderer = new StrikeRenderer(_window);
		m_renderer->init();
	}

	bool StrikeApp::IsRunning() const
	{
		return m_isRunning;
	}

	StrikeApp* StrikeApp::Instance()
	{
		return m_Instance;
	}

	void StrikeApp::OnQuit()
	{
		m_isRunning = false;
	}

	/*RenderableResourceController* StrikeApp::GetResourceController()
	{
		return resourceController;
	}*/

}
