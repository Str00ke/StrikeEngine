#pragma once
#include <d3d12.h>
#include "Core/ComPtr.hpp"
#include "Core/defines.hpp"

namespace StrikeEngine
{
	class Device;
	class Fence
	{
	public:
		Fence(Device* _device);
		Fence(const Fence&) = default;
		Fence(Fence&&) = delete;
		Fence& operator=(const Fence&) = delete;
		Fence& operator=(Fence&&) = delete;
		~Fence() = default;

		ID3D12Fence* getFenceAPI() const;
		const u64 getFenceValue() const;
		void increment();

	private:
		void createFence(Device* _device);

		ComPtr<ID3D12Fence> m_fence;
		u64 m_fenceValue;
	};
}


