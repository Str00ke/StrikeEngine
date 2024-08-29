#include "Renderer/CommandAllocator.hpp"
#include "Renderer/Device.hpp"
#include "Tools/D3DUtils.hpp"


StrikeEngine::CommandAllocator::CommandAllocator(Device* _device)
{
	createCmdAlloc(_device);
}

ID3D12CommandAllocator* StrikeEngine::CommandAllocator::getCommandAllocatorAPI() const
{
	return m_cmdAlloc.Get();
}

void StrikeEngine::CommandAllocator::reset() const
{
	D3D::Utils::throwIfFailed(m_cmdAlloc.Get()->Reset());
}

void StrikeEngine::CommandAllocator::createCmdAlloc(Device* _device)
{
	D3D::Utils::throwIfFailed(_device->getDeviceAPI()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAlloc)));
}
