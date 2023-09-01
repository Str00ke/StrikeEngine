#include <iostream>

#include "StrikeApp.hpp"
#include "StrikeRenderer.hpp"
#include "OS.hpp"
#include "Model.hpp"
#include "Time.hpp"
#include "InputSystem.hpp"
#include "Camera.hpp"

int main() {

	StrikeEngine::StrikeApp app;
	StrikeEngine::StrikeWindow window;
	StrikeEngine::StrikeRenderer rend(&window);
	StrikeEngine::Time::Init();
	StrikeEngine::InputSystem input;
	StrikeEngine::Camera cam;
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


	/*input.AddBinding(StrikeEngine::KeyCode::Z, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_forward += 0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::S, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_forward += -0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::D, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rightward += 0.25f;  });
	input.AddBinding(StrikeEngine::KeyCode::Q, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rightward += -0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::M, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rotX += 0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::K, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rotX -= 0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::O, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rotY += 0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::L, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rotY -= 0.25f; });*/

	input.AddBinding(StrikeEngine::KeyCode::Z, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.pos.x += 0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::S, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.pos.x += -0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::D, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.pos.y += 0.25f;  });
	input.AddBinding(StrikeEngine::KeyCode::Q, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.pos.y += -0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::M, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.pos.z += 0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::K, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.pos.z -= 0.25f; });



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