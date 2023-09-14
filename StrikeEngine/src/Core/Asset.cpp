#include "Core/Asset.hpp"

namespace StrikeEngine
{
	void Asset::SetPath(const std::string& assetPath)
	{
		m_assetPath = assetPath;
	}

	std::string& Asset::GetPath()
	{
		return m_assetPath;
	}

}

