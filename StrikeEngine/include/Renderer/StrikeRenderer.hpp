#pragma once
//#define VK_PROTOTYPES
//#define VK_USE_PLATFORM_WIN32_KHR
#include <vector>
#include "Core/OS.hpp"
#include <string>
#include <iostream>
#include "Tools/Tools.hpp"
#include "Math/Matrix4X4.hpp"
#include "Math/Math.hpp"
#include "Core/defines.hpp"
#include <dxgi.h>
#include <memory>
#include <vector>
#include "Buffer.hpp"
#include "Core/Blob.hpp"
#include "PipelineState.hpp"
#include <DirectXMath.h>

namespace StrikeEngine
{

	
	class Model;
	class Device;
	class CommandAllocator;
	class CommandList;
	class CommandQueue;
	class DescriptorHeap;
	class Swapchain;
	class Resource;
	class Fence;
	class RootSignature;
	class MeshGeometry;
	struct UniformBufferObject;
	class Camera;
	class StrikeRenderer
	{
	public:
		StrikeRenderer(StrikeWindow*);
		StrikeRenderer(const StrikeRenderer&) = delete;
		StrikeRenderer(StrikeRenderer&&) = delete;
		void operator=(const StrikeRenderer&) = delete;
		~StrikeRenderer();

		static StrikeRenderer* Instance();

		void init();

		StrikeWindow* GetStrikeWindow() const;
		Device* getDevice() const;
		const DXGI_FORMAT getBackBufferFormat() const;
		D3D12_CPU_DESCRIPTOR_HANDLE currentBackBufferView() const;
		ID3D12Resource* currentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView() const;

		float getAspectRatio() const;

		void calculateFrameStatistics();

		void Draw(f32 _deltaTime);

		template <typename T>
		inline void updateConstantBuffer(UploadBuffer<T>* _buffer, T _data, u32 _dataSizeInBytes) 
		{
			BYTE* mappedData = nullptr;
			_buffer->resource()->getResourceAPI()->Map(0, nullptr, reinterpret_pointer_cast<void**>(mappedData));
			memcpy(mappedData, &_data, _dataSizeInBytes);
			_buffer->resource()->getResourceAPI()->Unmap(0, nullptr);
			mappedData = nullptr;
		}
		void updateCameraConstantBuffer(Camera* _camera, UniformBufferObject* _ubo);

		CommandQueue* cmdQueue;
		CommandAllocator* cmdAlloc;
		CommandList* cmdList;
		DescriptorHeap* m_dsvHeap;
		DescriptorHeap* m_rtvHeap;

		u32 m_rtvDescriptorSize = 0; //Render Target View Descriptor


		const static u32 swapchainBufferCount = 2; // Double buffering | TODO: since this is precising double buffering, maybe create a enum or something like that
		
		float& getColorR() { return _colors[0]; };
		float& getColorG() { return _colors[1]; };
		float& getColorB() { return _colors[2]; };

		float posX = 0.0f;
		float posY = 0.0f;
		float posZ = 0.0f;
		float rotX = 0.0f;
		float rotY = 0.0f;

		float offX = 0.0f;
		float offY = 0.0f;
		float offZ = 0.0f;
		float offRotX = 0.0f;
		float offRotY = 0.0f;

		float m_theta = 1.5f * PI;
		float m_phi = PI / 4;
		float m_radius = 5.0f;
		float m_move = 0.0f;
		Vector2f m_lastMousePos = Vector2f(0.0f, 0.0f);
		DirectX::XMFLOAT4X4 m_proj = Matrix4X4::Identity4x4();

		inline void upR() { _colors[0] += 10.0f * m_strikeWin->time.deltaTime(); Math::Clamp(0.0f, 1.0f, _colors[0]); std::cout << _colors[0] << "\n"; };
		inline void downR() { _colors[0] -= 10.0f * m_strikeWin->time.deltaTime(); Math::Clamp(0.0f, 1.0f, _colors[0]); };

		void upG() { _colors[1] += 10.0f * m_strikeWin->time.deltaTime(); Math::Clamp(0.0f, 1.0f, _colors[1]); };
		void downG() { _colors[1] -= 10.0f * m_strikeWin->time.deltaTime(); Math::Clamp(0.0f, 1.0f, _colors[1]); };

		void upB() { _colors[2] += 10.0f * m_strikeWin->time.deltaTime(); Math::Clamp(0.0f, 1.0f, _colors[2]); };
		void downB() { _colors[2] -= 10.0f * m_strikeWin->time.deltaTime(); Math::Clamp(0.0f, 1.0f, _colors[2]); };

		void onMouseMove(float x, float y);

		inline const void StrikeRenderer::setActiveCamera(Camera& _camera)
		{
			m_activeCamera = &_camera;
		}

		inline const Camera* getActiveCamera() const { return m_activeCamera; }
	private:

		void buildDescriptorHeaps();
		void buildConstantBuffers();
		void buildRootSignature();
		void buildShadersAndInputLayout();
		void buildGeometry();
		void buildPSO();

		StrikeWindow* m_strikeWin;
		Device* m_device;
		u32 m_currBackBuffer = 0;

		RootSignature* m_rootSignature = nullptr;
		DescriptorHeap* m_cbvHeap = nullptr;

		std::unique_ptr<UploadBuffer<UniformBufferObject>> m_objectCB = nullptr;

		std::unique_ptr<MeshGeometry> m_boxGeo = nullptr;

		Blob m_vsByteCode = nullptr;
		Blob m_psByteCode = nullptr;

		//std::vector<D3D12_INPUT_ELEMENT_DESC[]> m_inputLayout;
		std::vector<std::vector<D3D12_INPUT_ELEMENT_DESC>> m_inputLayout;
		//D3D12_INPUT_ELEMENT_DESC* m_inputLayout;

		PipelineState* m_pso;

		u32 m_dsvDescriptorSize = 0; //Depth Stencil View Descriptor
		u32 m_cbvSrvDescriptorSize = 0; //UAV Descriptor

		Fence* m_fence;

		Swapchain* m_swapchain;
		//Resource* m_depthStencilBuffer;
		ComPtr<ID3D12Resource> m_depthStencilBuffer;

		void checkMSAAQualitySupport(u8 _qualityLevel);

		u32 dir = 1;
		f32 _colors[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		Camera* m_activeCamera = nullptr;
	protected:
		static StrikeRenderer* m_instance; //TODO: Remove?
		const DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM; // TODO: move
		const DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // TODO: move

	};


}

