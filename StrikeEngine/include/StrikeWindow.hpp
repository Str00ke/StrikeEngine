#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

class StrikeWindow
{
public:
	StrikeWindow();
	StrikeWindow(const StrikeWindow&) = delete;
	StrikeWindow(StrikeWindow&&) = delete;
	~StrikeWindow();

	int Init();

	//tmp??
	void OnSize(HWND hwnd, UINT flag, int width, int height);

	void ProcessWindowMessages();

private:
	
};

