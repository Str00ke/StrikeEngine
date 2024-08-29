#pragma once
#include "defines.hpp"
#include "Core/ComPtr.hpp"
#include <d3d12.h>
#include "Core/Blob.hpp"


namespace StrikeEngine
{
	class Device;
	class RootSignature
	{
	public:
		RootSignature() = default;
		RootSignature(const RootSignature&) = default;
		RootSignature(RootSignature&&) = delete;
		RootSignature& operator=(const RootSignature&) = delete;
		RootSignature& operator=(RootSignature&&) = delete;
		~RootSignature() = default;

		inline ID3D12RootSignature* getRootSignatureAPI() const { return m_rootSignature.Get(); };
		inline ComPtr<ID3D12RootSignature> getRootSignatureCom() const { return m_rootSignature; };

		void create(Device* _device, u32 _nodeMask, Blob& _SerializedRootSignature);
	private:
		ComPtr<ID3D12RootSignature> m_rootSignature;
	};
}



