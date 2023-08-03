#include <iostream>

#include "StrikeApp.hpp"
#include "OS.hpp"

int main() {

	StrikeWindow window;
	// Window creation
	if (!window.Create("01 - The Beginning")) {
		return -1;
	}
	return 0;
};