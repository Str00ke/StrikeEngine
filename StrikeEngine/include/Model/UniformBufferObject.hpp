#pragma once
#include "Math/Matrix4X4.hpp"
#include "Math/Vector4.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/hash.hpp"

namespace StrikeEngine
{
	__declspec(align(16))
		struct UniformBufferObject
	{
		Matrix4X4 model;
		Matrix4X4 view;
		Matrix4X4 proj;
		Vector4f camPos;
	};
}

