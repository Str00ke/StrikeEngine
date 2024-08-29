#pragma once
#include <d3d12.h>
#include "Core/ComPtr.hpp"

namespace StrikeEngine 
{
	class Device;
	class CommandAllocator
	{
	public:
		CommandAllocator(Device* _device);
		CommandAllocator(const CommandAllocator&) = default;
		CommandAllocator(CommandAllocator&&) = delete;
		CommandAllocator& operator=(const CommandAllocator&) = delete;
		CommandAllocator& operator=(CommandAllocator&&) = delete;
		~CommandAllocator() = default;

		ID3D12CommandAllocator* getCommandAllocatorAPI() const;
		void reset() const;

	private:
		void createCmdAlloc(Device* _device);

		ComPtr<ID3D12CommandAllocator> m_cmdAlloc;
	};
}

