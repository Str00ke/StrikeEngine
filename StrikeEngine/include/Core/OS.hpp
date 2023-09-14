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


namespace StrikeEngine
{
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
			virtual bool OnWindowSizeChanged() = 0;
			virtual bool Draw() = 0;

			virtual bool ReadyToDraw() const final
			{
				return m_CanRender;
			}

			Window() :
				m_CanRender(false) {}

			virtual ~Window() {}

		protected:
			bool m_CanRender;
		};


		struct WindowParameters
		{
			//#ifdef VK_USE_PLATFORM_WIN32_KHR
			HINSTANCE	Instance;
			HWND		Handle;

			WindowParameters() :
				Instance(),
				Handle()
			{}
			//#endif
		};
	}


	class StrikeWindow
	{
	public:
		StrikeWindow();
		StrikeWindow(const StrikeWindow&) = delete;
		StrikeWindow(StrikeWindow&&) = delete;
		~StrikeWindow();

		bool Create(const char* title, const float& width, const float& height);
		bool RenderingLoop(OS::Window& window) const;
		LRESULT HandleKeyMsg(MSG msg) const;
		OS::WindowParameters GetParams() const;

		void GetMousePosWindow(Vector2i* vec);

	private:
		OS::WindowParameters m_params;

	};

}
//#endif //OS_HEADER

