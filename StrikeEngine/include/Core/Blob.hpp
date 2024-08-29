#pragma once
#include "Core/defines.hpp"
#include "Core/ComPtr.hpp"
#include <D3DCompiler.h>

using Blob = ComPtr<ID3DBlob>;

//struct Blob
//{
//public:
//	inline ID3DBlob* getBlobApi() { return m_blob.Get(); };
//
//private:
//	ComPtr<ID3DBlob> m_blob;
//};
