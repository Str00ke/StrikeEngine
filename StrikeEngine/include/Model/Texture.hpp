#pragma once
#include <string>
#include "Core/Asset.hpp"




namespace StrikeEngine
{
	

	class StrikeRenderer;
	class Model;
	class Texture : public Asset
	{
	public:

		bool Create(StrikeRenderer* renderer, Model* model);
		bool CreateDepthTexture(StrikeRenderer* renderer);

	private:

	};

};

