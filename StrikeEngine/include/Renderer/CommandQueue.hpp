#pragma once
#include <d3d12.h>
#include "Core/ComPtr.hpp"


namespace StrikeEngine
{
	class Device;
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


	private:
		void createCommandQueue(Device* _device);
		void flush();

		ComPtr<ID3D12CommandQueue> m_cmdQueue;
	};
}




