#include <iostream>

#include "StrikeApp.hpp"
#include "StrikeRenderer.hpp"
#include "OS.hpp"
#include "Model.hpp"

int main() {

	StrikeEngine::StrikeApp app;
	StrikeEngine::StrikeWindow window;
	StrikeEngine::StrikeRenderer rend(&window);
	// Window creation
	if (!window.Create("Strike Test", 1200, 800)) {
		return -1;
	}

	StrikeEngine::Model model;
	model.GetMesh().SetPath("../Data/viking_room.obj");
	model.GetTexture().SetPath("../Data/viking_room.png");

	StrikeEngine::Model model2;
	model2.GetMesh().SetPath("../Data/Gear2.obj");
	model2.GetTexture().SetPath("../Data/Gear_2_BaseColor.png");

	StrikeEngine::StrikeRenderer::Instance()->toRend.push_back(&model);
	StrikeEngine::StrikeRenderer::Instance()->toRend.push_back(&model2);

	if (!rend.InitVulkan())
	{
		return -1;
	}

	if (!window.RenderingLoop(rend))
	{
		return -1;
	}
	return 0;
};