#pragma once
#include "Mesh.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include "Math/Vector3.hpp"

namespace StrikeEngine
{
	struct Cube_Params
	{
		float size;
		Vector3f position;
	};

	class PrimitiveShape_Cube : public Mesh
	{
	public:
		bool Init(StrikeRenderer* renderer, const Cube_Params& params);
	};

	class PrimitiveShape : public Mesh
	{
	public:
		template<typename ShapeType>
		//bool Init(StrikeRenderer* renderer, const Cube_Params& params);
		bool Init(ShapeType type, StrikeRenderer* renderer, const Cube_Params& params);
	};

	template<typename ShapeType>
	//inline bool PrimitiveShape::Init(StrikeRenderer* renderer, const Cube_Params& params)
	inline bool PrimitiveShape::Init(ShapeType type, StrikeRenderer* renderer, const Cube_Params& params)
	{
		type.Init(renderer, params);
		return true;
	}

}




