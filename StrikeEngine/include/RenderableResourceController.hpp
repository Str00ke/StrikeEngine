#pragma once
#include <vector>
#include "Renderable.hpp"

namespace StrikeEngine
{
	class RenderableResourceController
	{
	public:
		std::vector<Renderable*> renderedArray;
	};
}
