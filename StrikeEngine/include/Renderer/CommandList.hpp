#pragma once
#include <d3d12.h>
#include "Core/ComPtr.hpp"

namespace StrikeEngine
{
	class CommandQueue;
	class Device;
	class CommandAllocator;
	class PipelineState;
	class CommandList
	{
	public:
		CommandList(Device* _device, CommandAllocator* _commandAllocator);
		CommandList(const CommandList&) = default;
		CommandList(CommandList&&) = delete;
		CommandList& operator=(const CommandList&) = delete;
		CommandList& operator=(CommandList&&) = delete;
		~CommandList() = default;

		ID3D12CommandList* getCommandListAPI() const;

		void executeCommandList(CommandQueue* _commandQueue, UINT _count, const ID3D12CommandList** ppCommandList);
		void Reset(CommandAllocator* _commandAllocator, PipelineState* _pipelineState);

	private:
		void createCmdList(Device* _device, CommandAllocator* _commandAllocator);

		ComPtr<ID3D12CommandList> m_cmdList;
	};

}

