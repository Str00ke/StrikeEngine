#pragma once
#include <d3d12.h>
#include "Core/ComPtr.hpp"

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
		const UINT64 getFenceValue() const;

	private:
		void createFence(Device* _device);

		ComPtr<ID3D12Fence> m_fence;
		UINT64 m_fenceValue;
	};
}


