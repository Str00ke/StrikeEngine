#include "Model/Renderable.hpp"



namespace StrikeEngine
{
	//Renderable(StrikeRenderer* renderer, StrikeApp* app) :
	//	m_IsRendered(true),
	//	m_Renderer(renderer)
	//{
	//	//app->GetResourceController()->renderedArray.push_back(this);
	//}

	const bool Renderable::IsRendered() const
	{
		return m_IsRendered;
	}

	void Renderable::SetRender(bool enable)
	{
		m_IsRendered = enable;
	}

}
