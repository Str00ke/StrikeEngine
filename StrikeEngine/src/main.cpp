#include <iostream>

#include "StrikeApp.hpp"
#include "StrikeRenderer.hpp"
#include "OS.hpp"

int main() {

	StrikeEngine::StrikeWindow window;
	StrikeEngine::StrikeRenderer rend(&window);
	// Window creation
	if (!window.Create("Strike Test")) {
		return -1;
	}

	if (!rend.InitVulkan())
	{
		return -1;
	}

	if (!rend.CreateRenderingResources())
	{
		return -1;
	}

	if(!rend.CreateRenderPass())
	{
		return -1;
	}

	if (!rend.CreatePipeline())
	{
		return -1;
	}

	if (!rend.CreateVertexBuffer())
	{
		return -1;
	}

	if (!rend.CreateStagingBuffer())
	{
		return -1;
	}

	if (!rend.CopyVertexData())
	{
		return -1;
	}

	if (!window.RenderingLoop(rend))
	{
		return -1;
	}
	return 0;
};