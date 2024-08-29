#pragma once

//#ifndef UNICODE
//#define UNICODE
//#endif

#ifndef OS_HEADER
#define OS_HEADER

//#ifdef VK_USE_PLATFORM_WIN32_KHR

#endif //Platform used
#include "Math/Vector2.hpp"
#include <Windows.h>
#include "core/defines.hpp"
#include "Core/Time.hpp"

namespace StrikeEngine
{
	/////////////////////////////////////////////////////////////////////////////////////////////
	/// MANY OF THE OS STUFF NEEDS TO BE REWORKED ON
	/////////////////////////////////////////////////////////////////////////////////////////////
	namespace OS
	{
		//Use library depend of OS type

	//#ifdef VK_USE_PLATFORM_WIN32_KHR
		typedef HMODULE LibraryHandle;
		//#endif

			//Base class for basic Window operations
		class Window
		{
		public:
			bool OnWindowSizeChanged();
			bool Draw();

			virtual bool ReadyToDraw() const final
			{
				return m_CanRender;
			}

			Window() :
				m_CanRender(true) {}

			virtual ~Window() {}

		protected:
			bool m_CanRender;
		};


		struct WindowParameters
		{
			//#ifdef VK_USE_PLATFORM_WIN32_KHR
			HINSTANCE	Instance;
			HWND		Handle;
			u32			Width;
			u32			Height;

			WindowParameters() :
				Instance(),
				Handle()
			{}
			//#endif
		};
	}

	class StrikeRenderer;
	class StrikeWindow
	{
	public:
		StrikeWindow();
		StrikeWindow(const StrikeWindow&) = delete;
		StrikeWindow(StrikeWindow&&) = delete;
		~StrikeWindow();

		bool Create(const char* title, const float& width, const float& height);
		bool RenderingLoop(StrikeRenderer* _renderer); // TODO: the main loop needs to be refactored
		LRESULT HandleKeyMsg(MSG msg) const;
		OS::WindowParameters GetParams() const;

		void GetMousePosWindow(Vector2i* vec);
		Time time; // TODO: MOVE

	private:
		OS::WindowParameters m_params;
		OS::Window m_window;

	};

}
//#endif //OS_HEADER

