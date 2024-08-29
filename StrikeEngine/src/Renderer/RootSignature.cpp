#include "Renderer/RootSignature.hpp"
#include "Renderer/Device.hpp"
#include "Tools/D3DUtils.hpp"
#include "../Externals/d3dx12.h"


void StrikeEngine::RootSignature::create(StrikeEngine::Device* _device, u32 _nodeMask, Blob& _SerializedRootSignature)
{
	StrikeEngine::D3D::Utils::throwIfFailed(_device->getDeviceAPI()->CreateRootSignature(0, _SerializedRootSignature->GetBufferPointer(), _SerializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
}
