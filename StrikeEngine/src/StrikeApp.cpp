#include "StrikeApp.hpp"
#include "RenderableResourceController.hpp"


namespace StrikeEngine
{

	StrikeApp* StrikeApp::m_Instance = nullptr;;

	StrikeApp::StrikeApp()
	{
		m_isRunning = true;
		m_Instance = this;
		//resourceController = new RenderableResourceController();
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
