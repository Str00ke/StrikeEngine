#pragma once
#include "defines.hpp"
#include "Tools/Tools.hpp"

namespace StrikeEngine
{
	enum BufferType : i8
	{
		DEFAULT = 0,
		VERTEX = 1,
		INDEX = 2
	};

	class CommandList;
	class Device;
	class Resource;
	struct Buffer
	{
	public:
		Buffer() = default;
		Buffer(BufferType bufferType) :
			_bufferType(bufferType)
		{}
		Buffer(Resource* handle) :
			_handle(handle)
		{
			_bufferType = BufferType::DEFAULT;
		}


		const BufferType getBufferType() const { return _bufferType; };

		static const Buffer* CreateVertexBuffer();
		static const Buffer* CreateIndexBuffer();
		static const Buffer* CreateDefaultBuffer(Device* _device, CommandList* _graphicsCommandList, const void* _initData, u64 _byteSize, Resource* _uploadBuffer);
		inline Resource* getHandle() const { return _handle; };
	private:
		BufferType _bufferType;
		Resource* _handle;
	};

	template<typename T>
	class UploadBuffer
	{
	public:
		UploadBuffer(Device* _device, u32 _elementCount, bool _isConstant) :
			m_isConstant(_isConstant)
		{
			m_elementByteSize = sizeof(T);

			// Constant buffer elements need to be multiples of 256 bytes.
			// This is because the hardware can only view constant data
			// at m*256 byte offsets and of n*256 byte lengths.
			// typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
			// UINT64 OffsetInBytes; // multiple of 256
			// UINT SizeInBytes; // multiple of 256
			// } D3D12_CONSTANT_BUFFER_VIEW_DESC;
			if (_isConstant)
				m_elementByteSize = Tools::computeBufferConstantByteSize(sizeof(T));

			m_handle = Resource::Buffer(_device, D3D12_HEAP_TYPE_UPLOAD, D3D12_HEAP_FLAG_NONE, m_elementByteSize * _elementCount, D3D12_RESOURCE_STATE_GENERIC_READ);
			//m_mappedData = new Byte[m_elementByteSize * _elementCount];
			D3D::Utils::throwIfFailed(m_handle->getResourceAPI()->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedData)));

			// We do not need to unmap until we are done with the resource.
			// However, we must not write to the resource while it is in use by
			// the GPU (so we must use synchronization techniques)
		}

		UploadBuffer(const UploadBuffer&) = delete;
		UploadBuffer& operator=(const UploadBuffer&) = delete;
		~UploadBuffer()
		{
			if (m_handle != nullptr)
				m_handle->getResourceAPI()->Unmap(0, nullptr);

			m_mappedData = nullptr;
		}

		inline Resource* resource() const { return m_handle; };

		inline void copyData(u32 _elementIndex, const T& _data)
		{
			memcpy(&m_mappedData[_elementIndex * m_elementByteSize], &_data, sizeof(T));
		}

	private:
		Resource* m_handle;
		BYTE* m_mappedData;

		u32 m_elementByteSize;
		bool m_isConstant;
	};
}


