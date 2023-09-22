#pragma once
#include <string>

namespace StrikeEngine
{
	class Asset
	{
	public:
		Asset() :
			m_assetPath("")
		{}

		Asset(const std::string assetpath) :
			m_assetPath(assetpath)
		{}

		void SetPath(const std::string& assetPath);
		std::string& GetPath();

	protected:

	private:
		std::string m_assetPath = "NULL";
	};

}

