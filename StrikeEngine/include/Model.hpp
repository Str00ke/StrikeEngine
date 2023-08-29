#pragma once
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Renderable.hpp"

namespace StrikeEngine
{
	class Model : public Renderable
	{
	public:
		Model()
		{}

		Mesh& GetMesh();
		Texture& GetTexture();
		Texture& GetDepthTexture();

		bool Create();

	private:
		Mesh m_mesh;
		Texture m_tex;
		Texture m_depth;
	};

};



