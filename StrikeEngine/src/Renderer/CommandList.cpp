#include "Renderer/CommandList.hpp"
#include "Renderer/CommandQueue.hpp"
#include "Renderer/CommandAllocator.hpp"
#include "Tools/D3DUtils.hpp"
#include "Renderer/Device.hpp"
#include "Renderer/PipelineState.hpp"
#include "Renderer/Resource.hpp"
#include "Renderer/RootSignature.hpp"
#include "Renderer/DescriptorHeap.hpp"
#include "Core/defines.hpp"
#include "../Externals/d3dx12.h"


StrikeEngine::CommandList::CommandList(Device* _device, CommandAllocator* _commandAllocator)
{
	createCmdList(_device, _commandAllocator);
}

ID3D12GraphicsCommandList* StrikeEngine::CommandList::getCommandListAPI() const
{
	return m_cmdList.Get();
}

void StrikeEngine::CommandList::executeCommandList(CommandQueue* _commandQueue, UINT _count, const ID3D12CommandList** ppCommandList)
{
	ID3D12CommandList* ppCmdList[] = { m_cmdList.Get() };
	_commandQueue->getCommandQueueAPI()->ExecuteCommandLists(_count, ppCmdList);
}

void StrikeEngine::CommandList::Reset(CommandAllocator* _commandAllocator)
{
	//D3D::Utils::throwIfFailed(m_cmdList.Get()->Reset(_commandAllocator->getCommandAllocatorAPI(), nullptr));
	HRESULT hr = (m_cmdList.Get()->Reset(_commandAllocator->getCommandAllocatorAPI(), nullptr));
}

void StrikeEngine::CommandList::Reset(CommandAllocator* _commandAllocator, PipelineState* _pipelineState)
{
	D3D::Utils::throwIfFailed(m_cmdList.Get()->Reset(_commandAllocator->getCommandAllocatorAPI(), _pipelineState->getPipelineStateAPI()));
}

void StrikeEngine::CommandList::close()
{
	D3D::Utils::throwIfFailed(m_cmdList->Close());
}

void StrikeEngine::CommandList::resourceBarrier(u32 _count, D3D12_RESOURCE_BARRIER* _pBarriers)
{
	m_cmdList.Get()->ResourceBarrier(_count, _pBarriers);
}

D3D12_RESOURCE_BARRIER* StrikeEngine::CommandList::transition(Resource* _resource, D3D12_RESOURCE_STATES _stateBefore, D3D12_RESOURCE_STATES _stateAfter, u32 _subResource, D3D12_RESOURCE_BARRIER_FLAGS _barrierFlags)
{
	return &CD3DX12_RESOURCE_BARRIER::Transition(_resource->getResourceAPI(), _stateBefore, _stateAfter, _subResource, _barrierFlags);
}

void StrikeEngine::CommandList::RSSetViewport(u32 _count, D3D12_VIEWPORT* _pViewports)
{
	m_cmdList.Get()->RSSetViewports(_count, _pViewports);
}

void StrikeEngine::CommandList::RSSetScissorsRects(u32 _count, D3D12_RECT* _pScissors)
{
	m_cmdList.Get()->RSSetScissorRects(_count, _pScissors);
}

void StrikeEngine::CommandList::clearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuDesc, const f32 _rgba[4])
{
	m_cmdList.Get()->ClearRenderTargetView(_cpuDesc, _rgba, 0, nullptr);
}

void StrikeEngine::CommandList::clearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuDesc, const f32 _rgba[4], const u32 _numRects, const D3D12_RECT* _pRects)
{
	m_cmdList.Get()->ClearRenderTargetView(_cpuDesc, _rgba, _numRects, _pRects);
}

void StrikeEngine::CommandList::clearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuDesc, D3D12_CLEAR_FLAGS _clearFlags, f32 _depth, u8 _stencil, u32 _numRects, D3D12_RECT* _pRects)
{
	m_cmdList.Get()->ClearDepthStencilView(_cpuDesc, _clearFlags, _depth, _stencil, _numRects, _pRects);
}

void StrikeEngine::CommandList::OMSetRenderTargets(u32 _numTargetRenderDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* _pRenderTargetDescriptors, bool _handleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* _pDepthStencilDescriptors)
{
	m_cmdList.Get()->OMSetRenderTargets(_numTargetRenderDescriptors, _pRenderTargetDescriptors, _handleToDescriptorRange, _pDepthStencilDescriptors);
}

void StrikeEngine::CommandList::IASetPrimitiveTopology(const D3D_PRIMITIVE_TOPOLOGY _primitiveTopology) const
{
	m_cmdList.Get()->IASetPrimitiveTopology(_primitiveTopology);
}

void StrikeEngine::CommandList::IASetVertexBuffer(u32 _startSlot, u32 _numBuffers, const D3D12_VERTEX_BUFFER_VIEW* _pViews)
{
	m_cmdList.Get()->IASetVertexBuffers(_startSlot, _numBuffers, _pViews);
}

void StrikeEngine::CommandList::IASetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* _pViews)
{
	m_cmdList.Get()->IASetIndexBuffer(_pViews);
}

void StrikeEngine::CommandList::drawInstanced(u32 _vertexCountPerInstance, u32 _instanceCount, u32 _startVertexLocation, u32 _startInstanceLocation)
{
	m_cmdList.Get()->DrawInstanced(_vertexCountPerInstance, _instanceCount, _startVertexLocation, _startInstanceLocation);
}

void StrikeEngine::CommandList::drawIndexedInstanced(u32 _indexCountPerInstance, u32 _instanceCount, u32 _startIndexLocation, i32 _baseVertexLocation, u32 _startInstanceLocation)
{
	m_cmdList.Get()->DrawIndexedInstanced(_indexCountPerInstance, _instanceCount, _startIndexLocation, _baseVertexLocation, _startInstanceLocation);
}

void StrikeEngine::CommandList::setGraphicsRootDescriptorTable(u32 _rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE _baseDescriptor)
{
	m_cmdList.Get()->SetGraphicsRootDescriptorTable(_rootParameterIndex, _baseDescriptor);
}

void StrikeEngine::CommandList::setGraphicsRootSignature(StrikeEngine::RootSignature* _rootSignature)
{
	m_cmdList.Get()->SetGraphicsRootSignature(_rootSignature->getRootSignatureAPI());
}

void StrikeEngine::CommandList::setDescriptorHeaps(u32 _heapsCount, ID3D12DescriptorHeap* _desciptorHeaps)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { _desciptorHeaps };
	m_cmdList.Get()->SetDescriptorHeaps(_heapsCount, descriptorHeaps);
}

void StrikeEngine::CommandList::setPipelineState(PipelineState* _pipelineStateObject)
{
	m_cmdList.Get()->SetPipelineState(_pipelineStateObject->getPipelineStateAPI());
}

void StrikeEngine::CommandList::createCmdList(Device* _device, CommandAllocator* _commandAllocator)
{
	//TODO: ID3D12PipelineState is set to nullptr, change ASAP
	D3D::Utils::throwIfFailed(_device->getDeviceAPI()->CreateCommandList(_device->getNodeCount(), D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator->getCommandAllocatorAPI(), nullptr, IID_PPV_ARGS(&m_cmdList))); 
	m_cmdList.Get()->Close();
}
