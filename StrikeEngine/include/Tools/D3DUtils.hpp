#pragma once
#include <dxgi.h>
#include <exception>
#include <stdio.h>
#include "Core/ComPtr.hpp"

namespace StrikeEngine
{
	namespace D3D
	{
		namespace Utils 
		{
			class D3DUtils
			{
				static void logAdapters();
				static void logAdaptersOutput(IDXGIAdapter* _adapter);
				static void logOutputDisplayModes(IDXGIOutput* _output, DXGI_FORMAT _format);

				template<typename T>
				static void releaseCom(ComPtr<T> _com)
				{
					//_com->InternalRelease();
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

			// Helper utility converts D3D API failures into exceptions.
			inline void throwIfFailed(HRESULT hr)
			{
				if (FAILED(hr))
				{
					throw com_exception(hr);
				}
			}
		}
	}
}

