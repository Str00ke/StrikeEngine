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

		inline void SetName(const std::string& _name) { m_name = _name; };
		inline std::string& GetName() { return m_name; };

	protected:

	private:
		std::string m_name = "Name_Undifined";
		std::string m_assetPath = "NULL";
	};

}

