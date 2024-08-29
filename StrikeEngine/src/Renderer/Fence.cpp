#include "Renderer/Fence.hpp"
#include "Tools/D3DUtils.hpp"
#include "Renderer/Device.hpp"

StrikeEngine::Fence::Fence(Device* _device)
{
	createFence(_device);
}

ID3D12Fence* StrikeEngine::Fence::getFenceAPI() const
{
	return m_fence.Get();
}

const u64 StrikeEngine::Fence::getFenceValue() const
{
	return m_fenceValue;
}

void StrikeEngine::Fence::increment()
{
	m_fenceValue++;
}

void StrikeEngine::Fence::createFence(Device* _device)
{
	D3D::Utils::throwIfFailed(_device->getDeviceAPI()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
}
