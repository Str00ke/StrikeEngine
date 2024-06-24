#include "Renderer/CommandList.hpp"
#include "Renderer/CommandQueue.hpp"
#include "Renderer/CommandAllocator.hpp"
#include "Tools/D3DUtils.hpp"
#include "Renderer/Device.hpp"
#include "Renderer/PipelineState.hpp"


StrikeEngine::CommandList::CommandList(Device* _device, CommandAllocator* _commandAllocator)
{
	createCmdList(_device, _commandAllocator);
}

ID3D12CommandList* StrikeEngine::CommandList::getCommandListAPI() const
{
	return m_cmdList.Get();
}

void StrikeEngine::CommandList::executeCommandList(CommandQueue* _commandQueue, UINT _count, const ID3D12CommandList** ppCommandList)
{
	ID3D12CommandList* ppCmdList[] = { m_cmdList.Get() };
	_commandQueue->getCommandQueueAPI()->ExecuteCommandLists(_count, ppCmdList);
}

void StrikeEngine::CommandList::Reset(CommandAllocator* _commandAllocator, PipelineState* _pipelineState)
{
	_commandAllocator->getCommandAllocatorAPI()->Reset();
}

void StrikeEngine::CommandList::createCmdList(Device* _device, CommandAllocator* _commandAllocator)
{
	//TODO: ID3D12PipelineState is set to nullptr, change ASAP
	D3D::Utils::throwIfFailed(_device->getDeviceAPI()->CreateCommandList(_device->getNodeCount(), D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator->getCommandAllocatorAPI(), nullptr, IID_PPV_ARGS(&m_cmdList))); 
}
