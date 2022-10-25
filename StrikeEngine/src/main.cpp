#include <iostream>

#include "StrikeApp.hpp"
#include "StrikeWindow.hpp"

int main() {

	StrikeApp app;
	StrikeWindow window;
	window.Init();
	while (app.IsRunning())
	{
		window.ProcessWindowMessages();
	}
	return 0;
};