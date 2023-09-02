#pragma once
#include <functional>
#include <vector>
#include "Vector2.hpp"

namespace StrikeEngine
{
	enum KeyCode
	{
		A = 0x41,
		B = 0x42,
		C = 0x43,
		D = 0x44,
		E = 0x45,
		F = 0x46,
		G = 0x47,
		H = 0x48,
		I = 0x49,
		J = 0x4A,
		K = 0x4B,
		L = 0x4C,
		M = 0x4D,
		N = 0x4E,
		O = 0x4F,
		P = 0x50,
		Q = 0x51,
		R = 0x52,
		S = 0x53,
		T = 0x54,
		U = 0x55,
		V = 0x56,
		W = 0x57,
		X = 0x58,
		Y = 0x59,
		Z = 0x5A,
	};

	enum KeyStateType
	{
		ON_DOWN,
		ON_UP,
	};

	enum MouseCode
	{
		LEFT,
		RIGHT,
		MIDDLE
	};

	class InputSystem
	{
	public:
		InputSystem();
		void operator=(const InputSystem&) = delete;
		~InputSystem();
		static InputSystem* Instance();

		void AddBinding(const KeyCode& keyCode, const KeyStateType& state, std::function<void()> callback);
		void OnKeyDown(uint16_t keycode);
		void OnKeyUp(uint16_t keycode);
		void OnMouseBtnDown(const MouseCode& code);
		void OnMouseBtnUp(const MouseCode& code);
		void OnMouseMove(std::function<void()> callback);

		void GetMouseWindowPosition(Vector2i* vector);

		void MouseMoveCall();
	private:
		std::vector<std::pair<const KeyCode*, std::function<void()>>> m_keyDownCache;
		std::vector<std::pair<const KeyCode*, std::function<void()>>> m_keyUpCache;
		std::function<void()> m_mouseMoveCallback;

	protected:
		static InputSystem* m_instance;
	};
}