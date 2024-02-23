#pragma once
#define VK_PROTOTYPES
#define VK_USE_PLATFORM_WIN32_KHR
#include <vector>
#include "Core/OS.hpp"
#include <string>
#include <iostream>
#include "Tools/Tools.hpp"
#include "Math/Matrix4X4.hpp"



namespace StrikeEngine
{

	
	class Model;
	class StrikeRenderer : public OS::Window
	{
	public:
		StrikeRenderer(StrikeWindow*);
		StrikeRenderer(const StrikeRenderer&) = delete;
		StrikeRenderer(StrikeRenderer&&) = delete;
		void operator=(const StrikeRenderer&) = delete;
		~StrikeRenderer();

		static StrikeRenderer* Instance();


		StrikeWindow* GetStrikeWindow();

	private:
		StrikeWindow* m_strikeWin;

	protected:
		static StrikeRenderer* m_instance;
	};


}

