#include "StrikeWindow.hpp"
#include <exception>
#include <stdexcept>

#include "StrikeApp.hpp"


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
	
	switch(uMsg) //List of all messages (!WARNING!: HTTP address, so not secured): http://pinvoke.net/default.aspx/Constants.WM
	{
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		StrikeApp::Instance()->OnQuit();
		PostQuitMessage(0);
		return 0;
	}
	case WM_SIZE:
	{
		int width = LOWORD(lParam);  // Macro to get the low-order word.
		int height = HIWORD(lParam); // Macro to get the high-order word.

		auto window = reinterpret_cast<StrikeWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (window) window->OnSize(hwnd, (UINT)wParam, width, height);
		break;
	}
	default://If msg is not handled, this function apply msg by default 
	{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
	
}

StrikeWindow::StrikeWindow()
{

}

StrikeWindow::~StrikeWindow()
{

}

int StrikeWindow::Init()
{
	const wchar_t CLASS_NAME[] = L"Window Class";

	HINSTANCE hInstance = GetModuleHandle(NULL);

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

	/*
	wc.lpfnWndProc = &WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	*/
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.lpfnWndProc = WindowProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszMenuName = NULL;
	wc.hbrBackground = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	if (!RegisterClassEx(&wc)) // Or: RegisterClassExW()
	{
		// error! Use GetLastError() to find out why...
		return 0;
	}

	RECT rc = { 0,0,1024,768 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindowEx( //More info: https://learn.microsoft.com/fr-fr/windows/win32/api/winuser/nf-winuser-createwindowexa
		NULL,									// Optional window styles.
		CLASS_NAME,							// Window class
		L"Strike Window",					// Window text
		WS_VISIBLE | WS_OVERLAPPEDWINDOW,	// Window style

											//Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,								// Parent window
		NULL,								// Menu
		hInstance,							// Instance handle
		NULL								// Additional application data
	);

	if (!hwnd)
	{
		return 0;//std::exception("Window handle error!");
	}

	SetWindowLongPtr((HWND)hwnd, GWLP_USERDATA, (LONG_PTR)this);

	STARTUPINFO st = {};
	GetStartupInfo(&st);

	ShowWindow(hwnd, SW_SHOW); //More info of SW_SHOW: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
	UpdateWindow((HWND)hwnd);

}

void StrikeWindow::OnSize(HWND hwnd, UINT flag, int width, int height)
{
	//Handle window resizing
}

void StrikeWindow::ProcessWindowMessages()
{
	MSG msg = {};
	while (PeekMessage(&msg, HWND(), 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
