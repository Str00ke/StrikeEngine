#pragma once
//#include "RenderableResourceController.hpp"

namespace StrikeEngine
{
	class Renderable
	{
	public:
		Renderable() :
			m_IsRendered(true)
		{}
		const bool IsRendered() const;
	private:

	protected:
		bool m_IsRendered;
		void SetRender(bool enable);
	};
}


