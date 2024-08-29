#include "Renderer/DescriptorHeap.hpp"
#include "Tools/D3DUtils.hpp"
#include "Renderer/Device.hpp"

ID3D12DescriptorHeap* StrikeEngine::DescriptorHeap::getDescriptorHeapAPI() const
{
    return m_descHeap.Get();
}

ComPtr<ID3D12DescriptorHeap> StrikeEngine::DescriptorHeap::getDescriptorHeapCom() const
{
    return m_descHeap;
}

void StrikeEngine::DescriptorHeap::create(const D3D12_DESCRIPTOR_HEAP_DESC& _desc, Device* _device)
{
	D3D::Utils::throwIfFailed(_device->getDeviceAPI()->CreateDescriptorHeap(&_desc, IID_PPV_ARGS(&m_descHeap)));
}

