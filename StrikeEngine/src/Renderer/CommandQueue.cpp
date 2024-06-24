#include "Renderer/CommandQueue.hpp"
#include "Renderer/Device.hpp"
#include "Tools/D3DUtils.hpp"

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

void StrikeEngine::CommandQueue::flush()
{

}
