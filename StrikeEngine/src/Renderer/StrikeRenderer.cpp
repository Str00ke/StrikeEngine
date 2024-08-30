#include "Renderer/StrikeRenderer.hpp"
#include <vector>
#include "Core/Time.hpp"
#include "Model/Vertex.hpp"
#include "Model/RenderableResourceController.hpp"
#include "Model/Model.hpp"
#include "Model/Renderable.hpp"
#include "Model/Camera.hpp"
#include "Core/ComPtr.hpp"
#include <d3d12.h>
#include "Tools/D3DUtils.hpp"
#include "Renderer/Device.hpp"
#include "Renderer/CommandAllocator.hpp"
#include "Renderer/CommandList.hpp"
#include "Renderer/CommandQueue.hpp"
#include "Renderer/Swapchain.hpp"
#include "Renderer/DescriptorHeap.hpp"
#include "../Externals/d3dx12.h"
#include "Renderer/Resource.hpp"
#include "Renderer/Fence.hpp"
#include "Renderer/RootSignature.hpp"

#include <DirectXMath.h>

namespace StrikeEngine
{
	StrikeRenderer* StrikeRenderer::m_instance = nullptr;

	StrikeRenderer* StrikeRenderer::Instance()
	{
		if (m_instance == nullptr)
			throw std::runtime_error("Renderer instance is nullptr");

		return m_instance;
	}

	void StrikeRenderer::init()
	{
		//Try to create hardware device
		m_device = new Device();
		checkMSAAQualitySupport(4);
		cmdQueue = new CommandQueue(m_device);
		cmdAlloc = new CommandAllocator(m_device);
		cmdList = new CommandList(m_device, cmdAlloc);
		m_fence = new Fence(m_device);

		cmdList->Reset(cmdAlloc);

		buildDescriptorHeaps();
		buildConstantBuffers();
		buildRootSignature();
		buildShadersAndInputLayout();
		buildGeometry();
		buildPSO();

		m_swapchain = new Swapchain();
		m_swapchain->createSwapchain(*this);
		// Execute init commands
		cmdList->close();
		CommandList* cmdLists[] = { cmdList };
		cmdQueue->executeCmdLists(1, *cmdLists);

		// Wait until initialization is complete
		cmdQueue->flush(m_fence);

		DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * PI, getAspectRatio(), 1.0f, 1000.0f);
		XMStoreFloat4x4(&m_proj, P);
	}

	StrikeWindow* StrikeRenderer::GetStrikeWindow() const
	{
		return m_strikeWin;
	}

	Device* StrikeRenderer::getDevice() const
	{
		return m_device;
	}

	const DXGI_FORMAT StrikeRenderer::getBackBufferFormat() const
	{
		return m_backBufferFormat;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE StrikeRenderer::currentBackBufferView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_rtvHeap->getDescriptorHeapAPI()->GetCPUDescriptorHandleForHeapStart(), // handle start
			(INT)m_currBackBuffer, // index to offset
			(INT)m_rtvDescriptorSize // descriptor byte size
		);
	}

	ID3D12Resource* StrikeRenderer::currentBackBuffer() const
	{
		return m_swapchain->m_swapchainBuffer[m_currBackBuffer].Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE StrikeRenderer::depthStencilView() const
	{
		return m_dsvHeap->getDescriptorHeapAPI()->GetCPUDescriptorHandleForHeapStart();
	}

	float StrikeRenderer::getAspectRatio() const
	{
		return static_cast<float>(m_strikeWin->GetParams().Width) / m_strikeWin->GetParams().Height;
	}

	void StrikeRenderer::calculateFrameStatistics()
	{
		// Code computes the average frames per second, and also the
		// average time it takes to render one frame. These stats
		// are appended to the window caption bar

		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;

		// Compute averages over one second period
		if ((m_strikeWin->time.totalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt; // fps = frameCt / 1
			float milliSecPerFrame = 1000.0 / fps;

			std::wstring fpsStr = std::to_wstring(fps);
			std::wstring milliSecPerFrameStr = std::to_wstring(milliSecPerFrame);

			std::wstring windowText = L"Strike Engine -	FPS : " + fpsStr + L" mspf : " + milliSecPerFrameStr;

			SetWindowTextW(m_strikeWin->GetParams().Handle, windowText.c_str());

			// Reset for next average
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}

	void StrikeRenderer::Draw(f32 _deltaTime)
	{

		UniformBufferObject ubo;

		using namespace DirectX;
		XMFLOAT4X4 mWorld = Matrix4X4::Identity4x4();
		XMFLOAT4X4 mView = Matrix4X4::Identity4x4();
		//XMFLOAT4X4 mProj = Matrix4X4::Identity4x4();

		float x = m_radius * sinf(m_phi) * cosf(m_theta);
		float z = m_radius * sinf(m_phi) * sinf(m_theta);
		float y = m_radius * cosf(m_phi);
		//m_move += _deltaTime;
		//if (m_move == 0.0f) m_move += 0.01f;
		//std::cout << m_move << "\n";
		std::cout << x << " , " << x << " , " << z << std::endl;
		//XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
		XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
		XMVECTOR target = XMVectorZero();
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		// Build the view matrix.
		XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
		XMStoreFloat4x4(&mView, view);

		XMMATRIX world = XMLoadFloat4x4(&mWorld);
		XMMATRIX proj = XMLoadFloat4x4(&m_proj);
// 		XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * PI, getAspectRatio(), 1.0f, 1000.0f);
// 		XMStoreFloat4x4(&mProj, P);
		XMMATRIX worldViewProj = world * view * proj;

		// Update the constant buffer with the latest worldViewProj matrix.
		XMStoreFloat4x4(&ubo.WorldViewProj, XMMatrixTranspose(worldViewProj));
		m_objectCB->copyData(0, ubo);

		// Reuse the memory associated with command recording.
		// We can only reset when the associated command lists have finished
		// execution on the GPU.
		cmdAlloc->reset();

		// A command list can be reset after it has been added to the
		// command queue via ExecuteCommandList. Reusing the command list reuses memory
		cmdList->Reset(cmdAlloc, m_pso);

		// Set the viewport and scissor rect. This needs to be reset
		// whenever the command list is reset
		D3D12_VIEWPORT viewport;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<float>(m_strikeWin->GetParams().Width);
		viewport.Height = static_cast<float>(m_strikeWin->GetParams().Height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		cmdList->RSSetViewport(1, &viewport);

		D3D12_RECT scissors{ 0, 0, m_strikeWin->GetParams().Width, m_strikeWin->GetParams().Height };
		cmdList->RSSetScissorsRects(1, &scissors);
		
		// Indicate a state transition on the resource usage
		cmdList->resourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(currentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		_colors[0] += 1.0f * m_strikeWin->time.deltaTime(); 
		if (_colors[0] > 1.0f || _colors[0] < 0.0f) dir *= -1;
		_colors[1] = sin(_colors[0]);
		_colors[2] = cos(_colors[0]);

		// Clear the back buffer and depth buffer
		f32 col[4] = { 0.0f, 0.0f, 0.139f, 1.0 };
		auto back = currentBackBufferView();
		cmdList->clearRenderTargetView(currentBackBufferView(), col);
		cmdList->clearDepthStencilView(depthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		// Specify the buffer we are going to render into
		cmdList->OMSetRenderTargets(1, &currentBackBufferView(), true, &depthStencilView());

		ID3D12DescriptorHeap* descHeap[] = { m_cbvHeap->getDescriptorHeapAPI() };
		cmdList->setDescriptorHeaps(1, *descHeap);

		cmdList->setGraphicsRootSignature(m_rootSignature);
		cmdList->setPipelineState(m_pso);
		cmdList->IASetVertexBuffer(0, 1, &m_boxGeo->vertexBufferView());
		cmdList->IASetIndexBuffer(&m_boxGeo->indexBufferView());
		cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		cmdList->setGraphicsRootDescriptorTable(0, m_cbvHeap->getDescriptorHeapAPI()->GetGPUDescriptorHandleForHeapStart());

		cmdList->drawIndexedInstanced(m_boxGeo->_drawArgs["box"].indexCount, 1, 0, 0, 0);

		// Indicate a state transition on the resource usage
		cmdList->resourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(currentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		// Done recording commands
		cmdList->close();

		// Add the command list to the queue for execution
		cmdQueue->executeCmdLists(1, cmdList);
		calculateFrameStatistics();
		// Swap back and front buffer
		m_swapchain->present(0, 0);
		m_currBackBuffer = (m_currBackBuffer + 1) % swapchainBufferCount;

		// Wait until frame commands are complete
		cmdQueue->flush(m_fence);

		if (m_activeCamera) m_activeCamera->Update();
	}

	void StrikeRenderer::updateCameraConstantBuffer(Camera* _camera, UniformBufferObject* _ubo)
	{
	}

	

	void StrikeRenderer::onMouseMove(float x, float y)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - m_lastMousePos.x));
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - m_lastMousePos.y));

		//std::cout << dx << " , " << dy << std::endl;
		// Update angles based on input to orbit camera around box.
		m_theta += dx;
		m_phi += dy;

		// Restrict the angle mPhi.
		m_phi = Math::Clamp(m_phi, 0.1f, PIF - 0.1f);

		m_lastMousePos.x = x;
		m_lastMousePos.y = y;
	}

	void StrikeRenderer::buildDescriptorHeaps()
	{
		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;
		m_cbvHeap = new DescriptorHeap();
		m_cbvHeap->create(cbvHeapDesc, m_device);
		m_cbvSrvDescriptorSize = m_device->getDeviceAPI()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = swapchainBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		m_rtvHeap = new DescriptorHeap();
		m_rtvHeap->create(rtvHeapDesc, m_device);
		m_rtvDescriptorSize = m_device->getDeviceAPI()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		m_dsvHeap = new DescriptorHeap();
		m_dsvHeap->create(dsvHeapDesc, m_device);
		m_dsvDescriptorSize = m_device->getDeviceAPI()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		// TODO: Move
		// Create depth / stencil buffer and view
		D3D12_RESOURCE_DESC dsDesc;
		dsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsDesc.Alignment = 0;
		dsDesc.Width = m_strikeWin->GetParams().Width;
		dsDesc.Height = m_strikeWin->GetParams().Height;
		dsDesc.DepthOrArraySize = 1;
		dsDesc.MipLevels = 1;
		dsDesc.Format = m_depthStencilFormat;
		dsDesc.SampleDesc.Count = 1;
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = m_depthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		const D3D12_HEAP_TYPE type = D3D12_HEAP_TYPE_DEFAULT;
		const D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE;
		const D3D12_RESOURCE_STATES states = D3D12_RESOURCE_STATE_COMMON;
		Resource* res = new Resource(m_device, type, flags, dsDesc, states, optClear);
		D3D::Utils::throwIfFailed(m_device->getDeviceAPI()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(type),
			flags,
			&dsDesc,
			states,
			&optClear,
			IID_PPV_ARGS(&m_depthStencilBuffer)));

		// Create descriptor to mip level 0 of entire resource using the format of the resource
		D3D12_CPU_DESCRIPTOR_HANDLE view = m_dsvHeap->getDescriptorHeapAPI()->GetCPUDescriptorHandleForHeapStart();
		m_device->getDeviceAPI()->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, view);

		// Transition the resource from it's initial state to be used as a depth buffer
		cmdList->resourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	}

	void StrikeRenderer::buildConstantBuffers()
	{
		m_objectCB = std::make_unique<UploadBuffer<UniformBufferObject>>(m_device, 1, true);

		u32 objCBByteSize = StrikeEngine::Tools::computeBufferConstantByteSize(sizeof(UniformBufferObject));

		D3D12_GPU_VIRTUAL_ADDRESS cbAdress = m_objectCB->resource()->getResourceAPI()->GetGPUVirtualAddress();

		// Offset to the ith object constant buffer in the buffer. Here i = 0
		int boxConstBufferIndex = 0;
		cbAdress += boxConstBufferIndex * objCBByteSize;

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAdress;
		cbvDesc.SizeInBytes = objCBByteSize;

		m_device->getDeviceAPI()->CreateConstantBufferView(&cbvDesc, m_cbvHeap->getDescriptorHeapAPI()->GetCPUDescriptorHandleForHeapStart());
	}

	void StrikeRenderer::buildRootSignature()
	{
		// Shader programs typically require resources as input (constant
		// buffers, textures, samplers). The root signature defines the
		// resources the shader programs expect. If we think of the shader
		// programs as a function, and the input resources as function
		// parameters, then the root signature can be thought of as defining
		// the function signature.
		
		// Root parameter can be a table, root descriptor or root constants.
		CD3DX12_ROOT_PARAMETER slotRootParameter[1] = {};

		// Create a single descriptor table of CBV
		CD3DX12_DESCRIPTOR_RANGE cbvTable;
		cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

		// A root signature is an array of root parameters
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// Create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
		Blob serializedRootSig = nullptr;
		Blob errorBlob = nullptr;

		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		StrikeEngine::D3D::Utils::throwIfFailed(hr);
		m_rootSignature = new RootSignature();
		m_rootSignature->create(m_device, 0, serializedRootSig);
	}

	void StrikeRenderer::buildShadersAndInputLayout()
	{
		HRESULT hr = S_OK;
		m_vsByteCode = StrikeEngine::D3D::Utils::compileShader(L"C:/Users/cgarrigues/_Personal/StrikeEngine/StrikeEngine/src/Shaders/vertex.hlsl", nullptr, "VS", "vs_5_0");
		m_psByteCode = StrikeEngine::D3D::Utils::compileShader(L"C:/Users/cgarrigues/_Personal/StrikeEngine/StrikeEngine/src/Shaders/pixel.hlsl", nullptr, "PS", "ps_5_0");

		m_inputLayout = Vertex::getVertexStructureDescriptor();
		/*{
			*Vertex::getVertexStructureDescriptor()
		};*/
	}

	void StrikeRenderer::buildGeometry()
	{
		float size = 0.5f;
		std::array<Vertex, 8> vertices =
		{
			Vertex{Vector3f(-size, -size, -size),	Vector3f(1.0f, 0.0f, 0.0f),		/*Vector3f(1.0f, 1.0f, 1.0f)*/},
			Vertex{Vector3f(-size, size, -size),	Vector3f(0.0f, 1.0f, 0.0f),		/*Vector3f(1.0f, 1.0f, 1.0f)*/},
			Vertex{Vector3f(size, size, -size),		Vector3f(0.0f, 0.0f, 1.0f),		/*Vector3f(1.0f, 1.0f, 1.0f)*/},
			Vertex{Vector3f(size, -size, -size),	Vector3f(1.0f, 0.0f, 1.0f),		/*Vector3f(1.0f, 1.0f, 1.0f)*/},
			Vertex{Vector3f(-size, -size, size),	Vector3f(0.0f, 1.0f, 0.0f),		/*Vector3f(1.0f, 1.0f, 1.0f)*/},
			Vertex{Vector3f(-size, size, size),		Vector3f(1.0f, 1.0f, 0.0f),		/*Vector3f(1.0f, 1.0f, 1.0f)*/},
			Vertex{Vector3f(size, size, size),		Vector3f(0.0f, 1.0f, 1.0f),		/*Vector3f(1.0f, 1.0f, 1.0f)*/},
			Vertex{Vector3f(size, -size, size),		Vector3f(1.0f, 1.0f, 1.0f),		/*Vector3f(1.0f, 1.0f, 1.0f)*/},
		};

		//std::array<Vertex, 3> vertices =
		//{
		//	Vertex{Vector3f(0.0f, 0.5f, 0.0f),	Vector3f(1.0f, 1.0f, 1.0f),		/*Vector3f(1.0f, 1.0f, 1.0f)*/},
		//	Vertex{Vector3f(0.5f, 0.0f, 0.0f),	Vector3f(1.0f, 1.0f, 1.0f),		/*Vector3f(1.0f, 1.0f, 1.0f)*/},
		//	Vertex{Vector3f(-0.5f, 0.0f, 0.0f),	Vector3f(1.0f, 1.0f, 1.0f),		/*Vector3f(1.0f, 1.0f, 1.0f)*/},
		//};
		std::array<u32, 36> indices =
		{
			// front face
			0, 1, 2,
			0, 2, 3,
			// back face
			4, 6, 5,
			4, 7, 6,
			// left face
			4, 5, 1,
			4, 1, 0,
			// right face
			3, 2, 6,
			3, 6, 7,
			// top face
			1, 5, 6,
			1, 6, 2,
			// bottom face
			4, 0, 3,
			4, 3, 7
		};

		//std::array<u32, 3> indices =
		//{
		//	// front face
		//	0, 1, 2,
		//};


		const u32 vbufByteSize = (u32)vertices.size() * sizeof(Vertex);
		const u32 ibufByteSize = (u32)indices.size() * sizeof(u32);

		m_boxGeo = std::make_unique<MeshGeometry>();
		m_boxGeo->SetName("boxGeo");

		StrikeEngine::D3D::Utils::throwIfFailed(D3DCreateBlob(vbufByteSize, &m_boxGeo->vertexBufferCPU));
		CopyMemory(m_boxGeo->vertexBufferCPU->GetBufferPointer(), vertices.data(), vbufByteSize);

		StrikeEngine::D3D::Utils::throwIfFailed(D3DCreateBlob(ibufByteSize, &m_boxGeo->indexBufferCPU));
		CopyMemory(m_boxGeo->indexBufferCPU->GetBufferPointer(), indices.data(), ibufByteSize);

		m_boxGeo->vertexBufferGPU = Buffer::CreateDefaultBuffer(m_device, cmdList, vertices.data(), vbufByteSize, m_boxGeo->vertexBufferUploader)->getHandle();
		m_boxGeo->indexBufferGPU = Buffer::CreateDefaultBuffer(m_device, cmdList, indices.data(), ibufByteSize, m_boxGeo->indexBufferUploader)->getHandle();

		m_boxGeo->vertexByteStride = sizeof(Vertex);
		m_boxGeo->vertexBufferByteSize = vbufByteSize;
		m_boxGeo->indexFormat = DXGI_FORMAT_R32_UINT;
		m_boxGeo->indexBufferByteSize = ibufByteSize;

		SubmeshGeometry submesh;
		submesh.indexCount = (u32)indices.size();

		submesh.startIndexLocation = 0;
		submesh.baseVertexLocation = 0;

		m_boxGeo->_drawArgs["box"] = submesh;
	}

	void StrikeRenderer::buildPSO()
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		std::vector<D3D12_INPUT_ELEMENT_DESC> input = m_inputLayout[0];
		psoDesc.InputLayout = { input.data(), (u32)input.size() };
		psoDesc.pRootSignature = m_rootSignature->getRootSignatureAPI();
		psoDesc.VS =
		{
			reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
			m_vsByteCode->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
			m_psByteCode->GetBufferSize()
		};
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = m_backBufferFormat;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;
		psoDesc.DSVFormat = m_depthStencilFormat;
		m_pso = new PipelineState();
		m_pso->createGraphic(m_device, &psoDesc);
	}

	void StrikeRenderer::checkMSAAQualitySupport(u8 _qualityLevel)
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = m_backBufferFormat;
		msQualityLevels.SampleCount = _qualityLevel;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;

		D3D::Utils::throwIfFailed(m_device->getDeviceAPI()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels))); // TODO: impl RESULT type return
		/*std::cout << msQualityLevels.NumQualityLevels << "\n";
		std::cout << msQualityLevels.SampleCount << "\n";*/
	}

	

	StrikeRenderer::StrikeRenderer(StrikeWindow* strikeWindow) :
		m_strikeWin(strikeWindow)
	{
		if (m_instance != nullptr)
			throw std::runtime_error("Renderer already allocated");

		m_instance = this;
	}

	StrikeRenderer::~StrikeRenderer()
	{
		if (m_device != nullptr)
			cmdQueue->flush(m_fence);
		
	}

	
}