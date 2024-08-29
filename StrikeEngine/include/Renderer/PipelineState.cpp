#include "Renderer/PipelineState.hpp"
#include "Tools/D3DUtils.hpp"
#include "Renderer/Device.hpp"

ID3D12PipelineState* StrikeEngine::PipelineState::getPipelineStateAPI() const
{
    return m_pipelineState.Get();
}

void StrikeEngine::PipelineState::createGraphic(StrikeEngine::Device* _device, D3D12_GRAPHICS_PIPELINE_STATE_DESC* _desc)
{
	D3D::Utils::throwIfFailed(_device->getDeviceAPI()->CreateGraphicsPipelineState(_desc, IID_PPV_ARGS(&m_pipelineState)));
}

void StrikeEngine::PipelineState::createCompute(Device* _device, D3D12_COMPUTE_PIPELINE_STATE_DESC* _desc)
{
	D3D::Utils::throwIfFailed(_device->getDeviceAPI()->CreateComputePipelineState(_desc, IID_PPV_ARGS(&m_pipelineState)));
}
