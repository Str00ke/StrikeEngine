#pragma once
//#define VK_PROTOTYPES
//#define VK_USE_PLATFORM_WIN32_KHR
#include <vector>
#include "Core/OS.hpp"
#include <string>
#include <iostream>
#include "Tools/Tools.hpp"
#include "Math/Matrix4X4.hpp"
#include <dxgi1_4.h>



namespace StrikeEngine
{

	
	class Model;
	class Device;
	class StrikeRenderer/* : public OS::Window*/
	{
	public:
		StrikeRenderer(StrikeWindow*);
		StrikeRenderer(const StrikeRenderer&) = delete;
		StrikeRenderer(StrikeRenderer&&) = delete;
		void operator=(const StrikeRenderer&) = delete;
		~StrikeRenderer();

		static StrikeRenderer* Instance();

		void init();

		StrikeWindow* GetStrikeWindow();
		IDXGIFactory4* getFactory() const;

	private:
		StrikeWindow* m_strikeWin;
		IDXGIFactory4* m_dxgiFactory;
		Device* m_device;

	protected:
		static StrikeRenderer* m_instance; //TODO: Remove?
	};


}

