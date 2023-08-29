#pragma once

namespace StrikeEngine
{
	//class RenderableResourceController;
	class StrikeApp
	{
	public:
		StrikeApp();

		bool IsRunning() const;
		static StrikeApp* Instance();
		void OnQuit();
		//RenderableResourceController* GetResourceController();
	private:
		bool m_isRunning;
		static StrikeApp* m_Instance;
		//RenderableResourceController* resourceController;
	};

}

