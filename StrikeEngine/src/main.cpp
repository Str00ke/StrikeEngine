#include <iostream>

#include "Core/StrikeApp.hpp"
#include "Core/OS.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include "Model/Camera.hpp"
#include "Input/InputSystem.hpp"
#include "Math/Math.hpp"
int main() 
{
	StrikeEngine::StrikeApp app;
	StrikeEngine::StrikeWindow window;
	StrikeEngine::InputSystem inputs;
	// Window creation
	int width = 800;
	int height = 600;
	if (!window.Create("Strike Test", width, height)) {
		return -1;
	}
	StrikeEngine::StrikeRenderer rend(&window);
	StrikeEngine::StrikeRenderer* pRend = &rend;
	StrikeEngine::Camera cam;
	rend.init();
	rend.setActiveCamera(cam);
	bool running = true;
	
	//Vertical
	inputs.AddBinding(StrikeEngine::KeyCode::Q, StrikeEngine::KeyStateType::ON_DOWN, [&] { pRend->offX = -1.0f; });
	inputs.AddBinding(StrikeEngine::KeyCode::Q, StrikeEngine::KeyStateType::ON_UP, [&] { pRend->offX = 0.0f; });
	inputs.AddBinding(StrikeEngine::KeyCode::D, StrikeEngine::KeyStateType::ON_DOWN, [&] {  pRend->offX = 1.0f; });
	inputs.AddBinding(StrikeEngine::KeyCode::D, StrikeEngine::KeyStateType::ON_UP, [&] { pRend->offX = 0.0f; });

	//Horizontal
	inputs.AddBinding(StrikeEngine::KeyCode::S, StrikeEngine::KeyStateType::ON_DOWN, [&] { pRend->offY = -1.0f; });
	inputs.AddBinding(StrikeEngine::KeyCode::S, StrikeEngine::KeyStateType::ON_UP, [&] { pRend->offY = 0.0; });
	inputs.AddBinding(StrikeEngine::KeyCode::Z, StrikeEngine::KeyStateType::ON_DOWN, [&] {  pRend->offY = 1.0f; });
	inputs.AddBinding(StrikeEngine::KeyCode::Z, StrikeEngine::KeyStateType::ON_UP, [&] { pRend->offY = 0.0; });

	inputs.AddBinding(StrikeEngine::KeyCode::A, StrikeEngine::KeyStateType::ON_DOWN, [&] { pRend->offRotX = -1.0f; });
	inputs.AddBinding(StrikeEngine::KeyCode::A, StrikeEngine::KeyStateType::ON_UP, [&] { pRend->offRotX = 0.0; });
	inputs.AddBinding(StrikeEngine::KeyCode::E, StrikeEngine::KeyStateType::ON_DOWN, [&] {  pRend->offRotX = 1.0f; });
	inputs.AddBinding(StrikeEngine::KeyCode::E, StrikeEngine::KeyStateType::ON_UP, [&] { pRend->offRotX = 0.0; });

	inputs.AddBinding(StrikeEngine::KeyCode::W, StrikeEngine::KeyStateType::ON_DOWN, [&] { pRend->offRotY = -1.0f; });
	inputs.AddBinding(StrikeEngine::KeyCode::W, StrikeEngine::KeyStateType::ON_UP, [&] { pRend->offRotY = 0.0; });
	inputs.AddBinding(StrikeEngine::KeyCode::X, StrikeEngine::KeyStateType::ON_DOWN, [&] {  pRend->offRotY = 1.0f; });
	inputs.AddBinding(StrikeEngine::KeyCode::X, StrikeEngine::KeyStateType::ON_UP, [&] { pRend->offRotY = 0.0; });


	//inputs.OnMouseMove([] {std::cout << "Hello\n"; });

	while (window.RenderingLoop(&rend))
	{
		std::cout << running;
		if (!running) break;
	}
	return 0;
};