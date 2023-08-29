#include "Model.hpp"

namespace StrikeEngine
{
	Mesh& Model::GetMesh()
	{
		return m_mesh;
	}

	Texture& Model::GetTexture()
	{
		return m_tex;
	}

	bool Model::Create()
	{
		//Depth Texture
		if (!m_depth.CreateDepthTexture(StrikeRenderer::Instance()))
			return false;
		//--------------------------------------------------------------------------

		if (!m_tex.Create(StrikeRenderer::Instance()))
			return false;

		if (!m_mesh.Create(StrikeRenderer::Instance()))
			return false;

		return true;
	}

	Texture& Model::GetDepthTexture()
	{
		return m_depth;
	}

}

