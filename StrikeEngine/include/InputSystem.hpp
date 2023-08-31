#pragma once
#include <functional>
#include <vector>

namespace StrikeEngine
{
	enum KeyCode
	{
		A = 0x41,
		B = 0x42,
		C = 0x43,
		D = 0x44,
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
	private:
		std::vector<std::pair<const KeyCode*, std::function<void()>>> m_keyDownCache;
		std::vector<std::pair<const KeyCode*, std::function<void()>>> m_keyUpCache;

	protected:
		static InputSystem* m_instance;
	};
}
