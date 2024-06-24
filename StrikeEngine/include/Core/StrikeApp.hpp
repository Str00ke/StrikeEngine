#pragma once

namespace StrikeEngine
{
	//class RenderableResourceController;
	class StrikeRenderer;
	class StrikeWindow;
	class StrikeApp
	{
	public:
		StrikeApp();

		void init(StrikeWindow* _window);
		bool IsRunning() const;
		static StrikeApp* Instance();
		void OnQuit();
		//RenderableResourceController* GetResourceController();
	private:
		bool m_isRunning;
		static StrikeApp* m_Instance;
		StrikeRenderer* m_renderer;

		//RenderableResourceController* resourceController;
	};

}

