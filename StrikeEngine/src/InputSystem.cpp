#include "InputSystem.hpp"
#include <stdexcept>

namespace StrikeEngine
{
	InputSystem* InputSystem::m_instance = nullptr;

	InputSystem* InputSystem::Instance()
	{
		if (m_instance == nullptr)
			throw std::runtime_error("Renderer instance is nullptr");

		return m_instance;
	}

	InputSystem::InputSystem()
	{
		if (m_instance != nullptr)
			throw std::runtime_error("Renderer already allocated");

		m_instance = this;
	}

	InputSystem::~InputSystem()
	{

	}

	void InputSystem::AddBinding(const KeyCode& keyCode, const KeyStateType& state, std::function<void()> callback)
	{
		if (state == KeyStateType::ON_DOWN)
		{
			auto newPair = std::make_pair(&keyCode, callback);
			m_keyDownCache.push_back(newPair);
		}
		else if (state == KeyStateType::ON_UP)
		{
			auto newPair = std::make_pair(&keyCode, callback);
			m_keyUpCache.push_back(newPair);
		}
	}

	void InputSystem::OnKeyDown(uint16_t keycode)
	{
		for (size_t i = 0; i < m_keyDownCache.size(); ++i)
		{
			if (*m_keyDownCache[i].first == keycode)
			{
				m_keyDownCache[i].second();
				return;
			}
		}
	}

	void InputSystem::OnKeyUp(uint16_t keycode)
	{

	}

	void InputSystem::OnMouseBtnDown(const MouseCode& code)
	{

	}

	void InputSystem::OnMouseBtnUp(const MouseCode& code)
	{

	}

}

