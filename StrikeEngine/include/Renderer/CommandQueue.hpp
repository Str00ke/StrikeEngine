#pragma once
#include <d3d12.h>
#include "Core/ComPtr.hpp"
#include "Core/defines.hpp"

namespace StrikeEngine
{
	class Device;
	class CommandList;
	class Fence;
	class CommandQueue
	{
	public:
		CommandQueue(Device* _device);
		CommandQueue(const CommandQueue&) = default;
		CommandQueue(CommandQueue&&) = delete;
		CommandQueue& operator=(const CommandQueue&) = delete;
		CommandQueue& operator=(CommandQueue&&) = delete;
		~CommandQueue() = default;

		ID3D12CommandQueue* getCommandQueueAPI() const;

		void executeCmdLists(u32 _count, CommandList* _cmdLists); // _cmdLists ptr points to first elem of lsit array (like &arr[0])
		void flush(Fence* _fence);

	private:
		void createCommandQueue(Device* _device);

		ComPtr<ID3D12CommandQueue> m_cmdQueue;
	};
}




