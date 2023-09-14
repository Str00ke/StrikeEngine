#pragma once
#include <vector>
#include "Model/Renderable.hpp"

namespace StrikeEngine
{
	class RenderableResourceController
	{
	public:
		std::vector<Renderable*> renderedArray;
	};
}
