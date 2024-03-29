#include "Core/OS.hpp"
#include <exception>
#include <stdexcept>
#include <thread>
#include <chrono>
#include "Core/Time.hpp"
#include <stdio.h>
#include "Input/InputSystem.hpp"


namespace StrikeEngine
{

	OS::WindowParameters StrikeWindow::GetParams() const {
		return m_params;
	}

	void StrikeWindow::GetMousePosWindow(Vector2i* vec)
	{
		HWND hwnd = m_params.Handle;
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(hwnd, &p);
		vec->x = p.x;
		vec->y = p.y;
	}

	StrikeWindow::StrikeWindow() : m_params()
	{
	}

#define ENGINE_NAME "StrikeEngine"


	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		/*
		try
		{
			//GetWindowLong(hwnd,)
			switch (uMsg)
			{
			case WM_CREATE:
			{
				// Event fired when the window is created
				// collected here..

				break;
			}
			case WM_SIZE:
			{
				// Event fired when the window is resized
				auto window = reinterpret_cast<StrikeWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				//if (window) window->onSize(window->getClientSize());
				break;
			}
			case WM_SETFOCUS:
			{
				// Event fired when the window get focus
				auto window = reinterpret_cast<StrikeWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				//if (window) window->onFocus();
				break;
			}
			case WM_KILLFOCUS:
			{
				// Event fired when the window lost focus
				auto window = reinterpret_cast<StrikeWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				//window->onKillFocus();
				break;
			}
			case WM_DESTROY:
			{
				// Event fired when the window is destroyed
				auto window = reinterpret_cast<StrikeWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				//window->onDestroy();
				::PostQuitMessage(0);
				break;
			}
			case WM_MOVE:
			{
				// Event fired when the window is destroyed
				auto window = reinterpret_cast<StrikeWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				//if (window) window->onSize(window->getClientSize());
				//::PostQuitMessage(0);
				break;
			}
			case WM_CLOSE:
			{
				PostQuitMessage(0);
				break;
			}

			default:
				return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
		}
		catch (const std::runtime_error& e)
		{
			MessageBoxA(hwnd, e.what(), "PardCode | CXirectX 3D Game | Error", MB_ICONERROR | MB_OK);
			PostQuitMessage(0);
		}

		return NULL;
		*/

		switch (uMsg) //List of all messages (!WARNING!: HTTP address, so not secured): http://pinvoke.net/default.aspx/Constants.WM
		{
		case WM_DESTROY:
		{
			// close the application entirely
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			//StrikeApp::Instance()->OnQuit();
			//PostQuitMessage(0);
			//return 0;
			PostMessage(hwnd, WM_USER + 2, wParam, lParam);
			break;
		}
		case WM_SIZE:
		{
			int width = LOWORD(lParam);  // Macro to get the low-order word.
			int height = HIWORD(lParam); // Macro to get the high-order word.

			auto window = reinterpret_cast<StrikeWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			//if (window) window->OnSize(hwnd, (UINT)wParam, width, height);
			break;
		}
		case WM_EXITSIZEMOVE:
		{
			PostMessage(hwnd, WM_USER + 1, wParam, lParam);
			break;
		}
		default://If msg is not handled, this function apply msg by default 
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		}

	}

	StrikeWindow::~StrikeWindow()
	{
		if (m_params.Handle) {
			DestroyWindow(m_params.Handle);
		}

		if (m_params.Instance) {
			UnregisterClassA(ENGINE_NAME, m_params.Instance);
		}
	}


	bool StrikeWindow::Create(const char* title, const float& width, const float& height)
	{

		m_params.Instance = GetModuleHandle(nullptr);

		WNDCLASSEX wc = {};
		/* More info: https://learn.microsoft.com/fr-fr/windows/win32/api/winuser/ns-winuser-wndclassa
		UINT      style;
		WNDPROC   lpfnWndProc;
		int       cbClsExtra;
		int       cbWndExtra;
		HINSTANCE hInstance;
		HICON     hIcon;
		HCURSOR   hCursor;
		HBRUSH    hbrBackground;
		LPCSTR    lpszMenuName;
		LPCSTR    lpszClassName;
		*/

		wc.hInstance = m_params.Instance;
		wc.lpszClassName = ENGINE_NAME;
		wc.lpfnWndProc = WindowProc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		wc.lpszMenuName = NULL;
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		if (!RegisterClassEx(&wc)) // Or: RegisterClassExW()
		{
			// error! Use GetLastError() to find out why...
			return 0;
		}

		//RECT rc = { 0,0,1024,768 };
		//AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

		m_params.Handle = CreateWindow( //More info: https://learn.microsoft.com/fr-fr/windows/win32/api/winuser/nf-winuser-createwindowexa
			//NULL,									// Optional window styles.
			ENGINE_NAME,					// Window class
			title,							// Window text
			WS_VISIBLE | WS_OVERLAPPEDWINDOW,		// Window style

			//Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, width, height,

			NULL,									// Parent window
			NULL,									// Menu
			m_params.Instance,						// Instance handle
			NULL									// Additional application data
		);


		if (!m_params.Handle)
		{
			return false;//std::exception("Window handle error!");
		}

		SetWindowLongPtr(m_params.Handle, GWLP_USERDATA, (LONG_PTR)this);

		STARTUPINFO st = {};
		GetStartupInfo(&st);

		return true;
	}

	bool StrikeWindow::RenderingLoop(OS::Window& window) const
	{
		// Display window
		ShowWindow(m_params.Handle, SW_SHOWNORMAL);
		UpdateWindow(m_params.Handle);

		// Main message loop
		MSG message;
		bool loop = true;
		bool resize = false;
		bool res = true;

		while (loop) {
			Time::Update();
			if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
				//HandleKeyMsg(message);
				// Process events
				switch (message.message) {
					// Resize
				case WM_USER + 1:
					resize = true;
					break;
					// Close
				case WM_USER + 2:
					loop = false;
					break;

				//========Input Managment=========
				case WM_KEYDOWN: //FIXME: Not working on release
				{
					//printf("WM_KEYDOWN: 0x%x\n", message.wParam);
					InputSystem::Instance()->OnKeyDown(message.wParam);
					break;
				}
				case WM_KEYUP:
				{
					//printf("WM_KEYUP: 0x%x\n", message.wParam);
					InputSystem::Instance()->OnKeyUp(message.wParam);
					break;
				}
				case WM_CHAR:
				{
					//printf("WM_CHAR: %c\n", (wchar_t)message.wParam);
					break;
				}
				case WM_LBUTTONDOWN:
				{
					//printf("Left click dwn");
					break;
				}
				case WM_LBUTTONUP:
				{
					//printf("Left click up");
					break;
				}
				case WM_LBUTTONDBLCLK:
				{
					//printf("Left click dbl");
					break;
				}
				case WM_RBUTTONDOWN:
				{
					//printf("Right click dwn");
					break;
				}
				case WM_RBUTTONUP:
				{
					//printf("Right click up");
					break;
				}
				case WM_RBUTTONDBLCLK:
				{
					//printf("Right click dbl");
					break;
				}
				case WM_MBUTTONDOWN:
				{
					//printf("Mid click dwn");
					break;
				}
				case WM_MBUTTONUP:
				{
					//printf("Mid click up");
					break;
				}
				case WM_MBUTTONDBLCLK:
				{
					//printf("Mid click dbl");
					break;
				}
				case WM_MOUSEMOVE:
				{
					InputSystem::Instance()->MouseMoveCall();
					break;
				}
				//===================================
				}
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			else {
				// Resize
				if (resize) {
					resize = false;
					if (!window.OnWindowSizeChanged()) {
						res = false;
						break;
					}
				}
				// Draw
				if (window.ReadyToDraw()) {
					if (!window.Draw()) {
						res = false;
						break;
					}
				}
				else {
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			}
		}

		return res;
	}

	LRESULT StrikeWindow::HandleKeyMsg(MSG msg) const
	{
		switch (msg.message)
		{
		case WM_KEYDOWN:
		{
			//printf("WM_KEYDOWN: 0x%x\n", message.wParam);
			InputSystem::Instance()->OnKeyDown(msg.wParam);
			break;
		}
		case WM_KEYUP:
		{
			//printf("WM_KEYUP: 0x%x\n", message.wParam);
			InputSystem::Instance()->OnKeyUp(msg.wParam);
			break;
		}
		case WM_CHAR:
		{
			//printf("WM_CHAR: %c\n", (wchar_t)message.wParam);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			//printf("Left click dwn");
			break;
		}
		case WM_LBUTTONUP:
		{
			//printf("Left click up");
			break;
		}
		case WM_LBUTTONDBLCLK:
		{
			//printf("Left click dbl");
			break;
		}
		case WM_RBUTTONDOWN:
		{
			//printf("Right click dwn");
			break;
		}
		case WM_RBUTTONUP:
		{
			//printf("Right click up");
			break;
		}
		case WM_RBUTTONDBLCLK:
		{
			//printf("Right click dbl");
			break;
		}
		case WM_MBUTTONDOWN:
		{
			//printf("Mid click dwn");
			break;
		}
		case WM_MBUTTONUP:
		{
			//printf("Mid click up");
			break;
		}
		case WM_MBUTTONDBLCLK:
		{
			//printf("Mid click dbl");
			break;
		}
		case WM_MOUSEMOVE:
		{
			InputSystem::Instance()->MouseMoveCall();
			break;
		}
		default://If msg is not handled, this function apply msg by default 
		{
			return DefWindowProc(m_params.Handle, msg.message, msg.wParam, msg.lParam);
		}
		}
	}

}
