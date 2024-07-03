#include <iostream>

#include "Core/StrikeApp.hpp"
#include "Core/OS.hpp"
#include "Renderer/StrikeRenderer.hpp"
int main() 
{
	StrikeEngine::StrikeApp app;
	StrikeEngine::StrikeWindow window;
	// Window creation
	int width = 1200;
	int height = 1000;
	if (!window.Create("Strike Test", width, height)) {
		return -1;
	}
	StrikeEngine::StrikeRenderer rend(&window);
	rend.init();
	return 0;
};