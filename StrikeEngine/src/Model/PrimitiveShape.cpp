#include "Model/PrimitiveShape.hpp"


//bool StrikeEngine::PrimitiveShape_Cube::Create(StrikeRenderer* renderer, Cube_Params params)
//{
//	return true;
//}

bool StrikeEngine::PrimitiveShape_Cube::Init(StrikeRenderer* renderer, const Cube_Params& params)
{
	const float base = 1.0f;


	//const Vertex data[] =
	const std::vector<Vertex> data =
	{
		//Front
		{
			{ -base, base, -base },
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f }

		},
		{
			{ base,  base, -base },
			{ 1.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f }

		},
		{
			{ base, -base, -base },
			{ 1.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f }

		},
		{
			{ -base,  -base, -base },
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f }

		},
		//Back
		{
			{ -base, base, base },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f }

		},
		{
			{ base,  base, base },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f }

		},
		{
			{ base, -base, base },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f }

		},
		{
			{ -base,  -base, base },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f }

		},
	};

	//const uint32_t indices[] =
	const std::vector<uint32_t> indices =
	{
		//Front
		0, 1, 3,
		1, 2, 3,

		//Back
		5, 4, 6,
		4, 7, 6,

		//Top,
		4, 5, 0,
		5, 1, 0,

		//Bottom,
		3, 6, 7,
		3, 2, 6,

		//Left
		4, 0, 7,
		0, 3, 7,

		//Right
		1, 5, 2,
		5, 6, 2,
	};

	//std::vector<Vertex> _vBuffer(data, data + (sizeof(data) * 24));
	SetVertexBuffer(data);

	//std::vector<uint32_t> _indices(indices, indices + (sizeof(indices) * 36));
	SetIndexBuffer(indices);
	return false;
}
