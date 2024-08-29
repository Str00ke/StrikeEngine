#pragma once
#include <dxgi.h>
#include <exception>
#include <stdio.h>
#include <iostream>
#include "Core/ComPtr.hpp"
#include "d3dx12.h"
#include "Core/Blob.hpp"
#include <fstream>
#include "Core/defines.hpp"
#include <d3dcompiler.h>
#include <fstream>

namespace StrikeEngine
{
	namespace D3D
	{
		namespace Utils 
		{
			class D3DUtils
			{
			public:
				static void logAdapters();
				static void logAdaptersOutput(IDXGIAdapter* _adapter);
				static void logOutputDisplayModes(IDXGIOutput* _output, DXGI_FORMAT _format);

				template<typename T>
				static void releaseCom(ComPtr<T> _com)
				{
					//_com.InternalRelease();
				};
			};

			// Helper class for COM exceptions
			class com_exception : public std::exception
			{
			public:
				com_exception(HRESULT hr) : result(hr) {}

				const char* what() const noexcept override
				{
					static char s_str[64] = {};
					sprintf_s(s_str, "Failure with HRESULT of %08X",
						static_cast<unsigned int>(result));
					return s_str;
				}

			private:
				HRESULT result;
			};

			inline DWORD Win32FromHResult(HRESULT hr)
			{
				if ((hr & 0xFFFF0000) == MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0))
				{
					return HRESULT_CODE(hr);
				}

				if (hr == S_OK)
				{
					return ERROR_SUCCESS;
				}

				// Not a Win32 HRESULT so return a generic error code.
				return ERROR_CAN_NOT_COMPLETE;
			}

			// Helper utility converts D3D API failures into exceptions.
			// Use this for D3D functions that return HRESULT
			inline void throwIfFailed(HRESULT hr)
			{
				if (FAILED(hr))
				{
					std::cout << Win32FromHResult(hr) << "\n";
					throw com_exception(hr);
				}
			}

			class DxException 
			{
			public:
				DxException() = default;
				DxException(HRESULT _hr, const std::wstring& _funtionName, const std::wstring& _fileName, int _lineNbr);

				std::wstring toString() const;

				HRESULT _errorCode = S_OK;
				std::wstring m_functionName;
				std::wstring m_fileName;
				int m_lineNbr = -1;

				inline std::wstring AnsiToWstring(const std::string& input, DWORD locale = CP_UTF8)
				{
					wchar_t buf[8192] = { 0 };
					MultiByteToWideChar(locale, 0, input.c_str(), (int)input.length(), buf, ARRAYSIZE(buf));
					return buf;
				}
			};

			

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)												\
{																		\
	wchar_t buf[8192] = { 0 };											\
	HRESULT hr__ = (x);													\
	std::wstring wfn = AnsiToWstring(__FILE__);							\
	if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); }	\
}
#endif // !ThrowIfFailed

			inline ComPtr<ID3DBlob> compileShader(const std::wstring& _filename, const D3D_SHADER_MACRO* _defines, const std::string& _entrypoint, const std::string& _target)
			{
				std::ifstream file(_filename.c_str());
				if (!file.good())
				{
					std::cout << "Shader file not existing! Check your filepath!" << std::endl;
					throw std::invalid_argument("Shader file not existing! Check your filepath!");
				}

				// Use debug flags in debug mode
				u32 compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG) 
				compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
				HRESULT hr = S_OK;

				ComPtr<ID3DBlob> byteCode = nullptr;
				ComPtr<ID3DBlob> errors;

				hr = D3DCompileFromFile(_filename.c_str(), _defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, _entrypoint.c_str(), _target.c_str(), compileFlags, 0, &byteCode, &errors);

				// Output errors to debug window
				if (errors != nullptr) OutputDebugStringA((char*)errors->GetBufferPointer());

				throwIfFailed(hr);

				return byteCode;
			}


			inline Blob* loadBinary(const std::wstring& _filename)
			{
				std::ifstream fileStream(_filename, std::ios::binary);


				fileStream.seekg(0, std::ios_base::end);
				std::ifstream::pos_type size = (u32)fileStream.tellg();
				fileStream.seekg(0, std::ios_base::beg);

				Blob* blob;
				throwIfFailed(D3DCreateBlob(size, blob->GetAddressOf()));

				fileStream.read((char*)blob->Get()->GetBufferPointer(), size);
				fileStream.close();

				return blob;
			}
		}
	}
}

