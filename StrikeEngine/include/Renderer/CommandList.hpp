#pragma once
#include <d3d12.h>
#include "Core/ComPtr.hpp"
#include "Core/defines.hpp"


namespace StrikeEngine
{
	class CommandQueue;
	class Device;
	class CommandAllocator;
	class PipelineState;
	class Resource;
	class RootSignature;
	class CommandList
	{
	public:
		CommandList(Device* _device, CommandAllocator* _commandAllocator);
		CommandList(const CommandList&) = default;
		CommandList(CommandList&&) = delete;
		CommandList& operator=(const CommandList&) = delete;
		CommandList& operator=(CommandList&&) = delete;
		~CommandList() = default;

		ID3D12GraphicsCommandList* getCommandListAPI() const;

		void executeCommandList(CommandQueue* _commandQueue, UINT _count, const ID3D12CommandList** ppCommandList);
		void Reset(CommandAllocator* _commandAllocator);
		void Reset(CommandAllocator* _commandAllocator, PipelineState* _pipelineState);
		void close();

		// Commands
		void resourceBarrier(u32 _count, D3D12_RESOURCE_BARRIER* _pBarriers);
		void RSSetViewport(u32 _count, D3D12_VIEWPORT* _pViewports);
		void RSSetScissorsRects(u32 _count, D3D12_RECT* _pScissors);
		void clearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuDesc, const f32 _rgba[4]); // Clear rtv without precising rects, so whole viewport
		void clearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuDesc, const f32 _rgba[4], const u32 _numRects, const D3D12_RECT* _pRects);
		void clearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuDesc, D3D12_CLEAR_FLAGS _clearFlags, f32 _depth, u8 _stencil, u32 _numRects, D3D12_RECT* _pRects);
		void OMSetRenderTargets(u32 _numTargetRenderDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* _pRenderTargetDescriptors, bool _handleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* _pDepthStencilDescriptors);
		void IASetPrimitiveTopology(const D3D_PRIMITIVE_TOPOLOGY _primitiveTopology) const;
		void IASetVertexBuffer(u32 _startSlot, u32 _numBuffers, const D3D12_VERTEX_BUFFER_VIEW* _pViews);
		void IASetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* _pViews);
		void drawInstanced(u32 _vertexCountPerInstance, u32 _instanceCount, u32 _startVertexLocation, u32 _startInstanceLocation);
		void drawIndexedInstanced(u32 _indexCountPerInstance, u32 _instanceCount, u32 _startIndexLocation, i32 _baseVertexLocation, u32 _startInstanceLocation);
		void setGraphicsRootDescriptorTable(u32 _rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE _baseDescriptor);
		void setGraphicsRootSignature(RootSignature* _rootSignature);
		void setDescriptorHeaps(u32 _heapsCount, ID3D12DescriptorHeap* _desciptorHeaps);
		void setPipelineState(PipelineState* _pipelineStateObject);

		D3D12_RESOURCE_BARRIER* transition(Resource* _resource, D3D12_RESOURCE_STATES _stateBefore, D3D12_RESOURCE_STATES _stateAfter, u32 _subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_BARRIER_FLAGS _barrierFlags = D3D12_RESOURCE_BARRIER_FLAG_NONE);

	private:
		void createCmdList(Device* _device, CommandAllocator* _commandAllocator);

		ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	};

}

