#include <iostream>

#include "Core/StrikeApp.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include "Core/OS.hpp"
#include "Model/Model.hpp"
#include "Core/Time.hpp"
#include "Input/InputSystem.hpp"
#include "Model/Camera.hpp"
#include "Model/PrimitiveShape.hpp"

int main() {

	StrikeEngine::StrikeApp app;
	StrikeEngine::StrikeWindow window;
	//StrikeEngine::StrikeRenderer rend(&window);
	StrikeEngine::Time::Init();
	StrikeEngine::InputSystem input;
	StrikeEngine::Camera cam;
	// Window creation
	int width = 1200;
	int height = 1000;
	if (!window.Create("Strike Test", width, height)) {
		return -1;
	}

	StrikeEngine::Model model;
	model.GetMesh().SetPath("C:/Users/cgarrigues/_Personal/StrikeEngine/StrikeEngine/Data/viking_room.obj");
	model.GetTexture().SetPath("C:/Users/cgarrigues/_Personal/StrikeEngine/StrikeEngine/Data/viking_room.png");
	model.GetMesh().SetVertexColor(Vector3f(1.0f, 1.0f, 1.0f));
	//StrikeEngine::StrikeRenderer::Instance()->toRend.push_back(&model);

	/*StrikeEngine::Model model2;
	model2.GetMesh().SetPath("C:/Users/cgarrigues/_Personal/StrikeEngine/StrikeEngine/Data/Gear2.obj");
	model2.GetTexture().SetPath("C:/Users/cgarrigues/_Personal/StrikeEngine/StrikeEngine/Data/Gear_2_BaseColor.png");

	StrikeEngine::StrikeRenderer::Instance()->toRend.push_back(&model2);*/

	StrikeEngine::PrimitiveShape_Cube cube;
	StrikeEngine::Cube_Params params{ 1.0f, {0.0f, 0.0f, 0.0f} };
	//shape.Init<StrikeEngine::PrimitiveShape_Cube>(rend, params);
	//shape.Init(StrikeEngine::PrimitiveShape_Cube, rend, params);
	//cube.Init(&rend, params);
	StrikeEngine::Model modelPrim;
	modelPrim.SetMesh(cube);
	//StrikeEngine::StrikeRenderer::Instance()->toRend.push_back(&modelPrim);


	input.AddBinding(StrikeEngine::KeyCode::Z, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_forward += 0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::S, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_forward += -0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::D, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rightward += 0.25f;  });
	input.AddBinding(StrikeEngine::KeyCode::Q, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rightward += -0.25f; });

	input.AddBinding(StrikeEngine::KeyCode::Z, StrikeEngine::KeyStateType::ON_UP, [&] { cam.m_forward = 0.0f; });
	input.AddBinding(StrikeEngine::KeyCode::S, StrikeEngine::KeyStateType::ON_UP, [&] { cam.m_forward = 0.0f; });
	input.AddBinding(StrikeEngine::KeyCode::D, StrikeEngine::KeyStateType::ON_UP, [&] { cam.m_rightward = 0.0f;  });
	input.AddBinding(StrikeEngine::KeyCode::Q, StrikeEngine::KeyStateType::ON_UP, [&] { cam.m_rightward = 0.0f; });

// 	input.AddBinding(StrikeEngine::KeyCode::Z, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.pos.x += 0.25f; });
// 	input.AddBinding(StrikeEngine::KeyCode::S, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.pos.x += -0.25f; });
// 	input.AddBinding(StrikeEngine::KeyCode::D, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.pos.y += 0.25f;  });
// 	input.AddBinding(StrikeEngine::KeyCode::Q, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.pos.y += -0.25f; });
	input.AddBinding(StrikeEngine::KeyCode::B, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.SetFOV(cam.GetFOV() + 0.1f); });
	input.AddBinding(StrikeEngine::KeyCode::N, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.SetFOV(cam.GetFOV() - 0.1f); });
	//input.AddBinding(StrikeEngine::KeyCode::O, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.SetNearClip(cam.GetNearClip() + 0.1f); });
	//input.AddBinding(StrikeEngine::KeyCode::L, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.SetNearClip(cam.GetNearClip() - 0.1f); });
	//input.AddBinding(StrikeEngine::KeyCode::K, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.SetFarClip(cam.GetFarClip() + 0.1f); });
	//input.AddBinding(StrikeEngine::KeyCode::M, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.SetFarClip(cam.GetFarClip() - 0.1f); });

	/*input.AddBinding(StrikeEngine::KeyCode::O, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rotY -= 0.1f; });
	input.AddBinding(StrikeEngine::KeyCode::L, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rotY += 0.1f; });
	input.AddBinding(StrikeEngine::KeyCode::K, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rotX -= 0.1f; });
	input.AddBinding(StrikeEngine::KeyCode::M, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rotX += 0.1f; });*/

	input.AddBinding(StrikeEngine::KeyCode::O, StrikeEngine::KeyStateType::ON_DOWN, [&] { model.transform.m_scale += Vector3f(1.0f, 1.0f, 1.0f); });
	input.AddBinding(StrikeEngine::KeyCode::L, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rotY += 0.1f; });
	input.AddBinding(StrikeEngine::KeyCode::K, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rotX -= 0.1f; });
	input.AddBinding(StrikeEngine::KeyCode::M, StrikeEngine::KeyStateType::ON_DOWN, [&] { cam.m_rotX += 0.1f; });

	Vector2i prevPos;
	float camSpeed = 1.0f;
	input.OnMouseMove([&] 
		{ 
			Vector2i pos;
			input.GetMouseWindowPosition(&pos);
			cam.m_rotX += (prevPos.y - pos.y) * StrikeEngine::Time::DeltaTime() * camSpeed;
			cam.m_rotY += (prevPos.x - pos.x) * StrikeEngine::Time::DeltaTime() * camSpeed;
			prevPos = pos;
		});

	/*if (!window.RenderingLoop(rend))
	{
		return -1;
	}*/
	return 0;
};