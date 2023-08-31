#include <iostream>

#include "StrikeApp.hpp"
#include "StrikeRenderer.hpp"
#include "OS.hpp"
#include "Model.hpp"
#include "Time.hpp"
#include "InputSystem.hpp"

int main() {

	StrikeEngine::StrikeApp app;
	StrikeEngine::StrikeWindow window;
	StrikeEngine::StrikeRenderer rend(&window);
	StrikeEngine::Time::Init();
	StrikeEngine::InputSystem input;
	// Window creation
	if (!window.Create("Strike Test", 800, 600)) {
		return -1;
	}

	StrikeEngine::Model model;
	model.GetMesh().SetPath("C:/Users/Charlelie/Desktop/Engines/StrikeEngine/StrikeEngine/Data/viking_room.obj");
	model.GetTexture().SetPath("C:/Users/Charlelie/Desktop/Engines/StrikeEngine/StrikeEngine/Data/viking_room.png");

	StrikeEngine::Model model2;
	model2.GetMesh().SetPath("C:/Users/Charlelie/Desktop/Engines/StrikeEngine/StrikeEngine/Data/Gear2.obj");
	model2.GetTexture().SetPath("C:/Users/Charlelie/Desktop/Engines/StrikeEngine/StrikeEngine/Data/Gear_2_BaseColor.png");

	StrikeEngine::StrikeRenderer::Instance()->toRend.push_back(&model);
	StrikeEngine::StrikeRenderer::Instance()->toRend.push_back(&model2);

	float speed;

	input.AddBinding(StrikeEngine::KeyCode::A, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.testAngle += 0.1f * speed; });
	input.AddBinding(StrikeEngine::KeyCode::B, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.testAngle -= 0.1f * speed; });
	input.AddBinding(StrikeEngine::KeyCode::C, StrikeEngine::KeyStateType::ON_DOWN, [&] { speed -= 0.5f; });
	input.AddBinding(StrikeEngine::KeyCode::D, StrikeEngine::KeyStateType::ON_DOWN, [&] { speed -= 0.5f; });

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