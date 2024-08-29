#include "Renderer/CommandQueue.hpp"
#include "Renderer/Device.hpp"
#include "Tools/D3DUtils.hpp"
#include "Renderer/CommandList.hpp"
#include "Renderer/Fence.hpp"

StrikeEngine::CommandQueue::CommandQueue(Device* _device)
{
	createCommandQueue(_device);
}

ID3D12CommandQueue* StrikeEngine::CommandQueue::getCommandQueueAPI() const
{
	return m_cmdQueue.Get();
}

void StrikeEngine::CommandQueue::createCommandQueue(Device* _device)
{
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	D3D::Utils::throwIfFailed(_device->getDeviceAPI()->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue)));
}

void StrikeEngine::CommandQueue::flush(Fence* _fence)
{
	_fence->increment();
	auto fenceApi = _fence->getFenceAPI();
	u64 fenceValue = _fence->getFenceValue();

	// Add an instruction to the command queue to set a new fence point.
	// Because we are on the GPU timeline, the new fence point won’t be
	// set until the GPU finishes processing all the commands prior to
	// this Signal().
	D3D::Utils::throwIfFailed(m_cmdQueue->Signal(fenceApi, fenceValue));

	// Wait until the GPU has completed commands up to this fence point.
	if (fenceApi->GetCompletedValue() < fenceValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence
		D3D::Utils::throwIfFailed(fenceApi->SetEventOnCompletion(fenceValue, eventHandle));

		// Wait until the GPU hits current fence event is fired
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void StrikeEngine::CommandQueue::executeCmdLists(u32 _count, CommandList* _cmdLists)
{
	ID3D12CommandList* list[] = { _cmdLists->getCommandListAPI() };
	m_cmdQueue->ExecuteCommandLists(_count, list); // TODO: See __countof()
}
