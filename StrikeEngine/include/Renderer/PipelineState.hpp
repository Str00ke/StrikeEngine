#pragma once
#include <d3d12.h>
#include "Core/ComPtr.hpp"
#include "Core/defines.hpp"


namespace StrikeEngine
{
	class Device;
	class PipelineState
	{
	public:
		PipelineState() = default;
		PipelineState(const PipelineState&) = default;
		PipelineState(PipelineState&&) = delete;
		PipelineState& operator=(const PipelineState&) = delete;
		PipelineState& operator=(PipelineState&&) = delete;
		~PipelineState() = default;

		ID3D12PipelineState* getPipelineStateAPI() const;
		inline ComPtr<ID3D12PipelineState> getPipelineStateCOM() const { return m_pipelineState; };

		void createGraphic(Device* _device, D3D12_GRAPHICS_PIPELINE_STATE_DESC* _desc);
		void createCompute(Device* _device, D3D12_COMPUTE_PIPELINE_STATE_DESC* _desc);
	private:
		ComPtr<ID3D12PipelineState> m_pipelineState;
	};

}

