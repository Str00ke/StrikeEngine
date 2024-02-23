#pragma once
#include "Vector3.hpp"

class Transform
{
public:
	Vector3f m_position = { 0.0f, 0.0f , 0.0f };
	Vector3f m_rotation = { 0.0f, 0.0f , 0.0f };
	Vector3f m_scale = { 1.0f, 1.0f , 1.0f };
};

