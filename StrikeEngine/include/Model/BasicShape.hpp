#pragma once
#include "Model.hpp"

namespace StrikeEngine
{
	class BasicShape : public Model
	{
	public:
		enum ShapeType
		{
			SQUARE,
			SPHERE,
			CONE,
			CYLINDER
		};
	};
}

