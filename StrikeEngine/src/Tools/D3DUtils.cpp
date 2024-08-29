#include "Tools/D3DUtils.hpp"
#include <vector>
#include <string>
#include "Renderer/StrikeRenderer.hpp"
#include "defines.hpp"
#include <iostream>

void StrikeEngine::D3D::Utils::D3DUtils::logAdapters()
{
	u32 i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;

	auto factory = StrikeRenderer::Instance()->getDevice()->getFactory();
	while (factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"///Adapter: ";
		text += desc.Description;
		text += L"\n";
		OutputDebugStringW(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}
	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		logAdaptersOutput(adapterList[i]);
		releaseCom<IDXGIAdapter>(adapterList[i]);
	}
}

void StrikeEngine::D3D::Utils::D3DUtils::logAdaptersOutput(IDXGIAdapter* _adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (_adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"///Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugStringW(text.c_str());
		std::wcout << text << "\n";

		logOutputDisplayModes(output, DXGI_FORMAT_B8G8R8A8_UNORM);
		
		releaseCom<IDXGIOutput>(output);

		++i;
	}
}

void StrikeEngine::D3D::Utils::D3DUtils::logOutputDisplayModes(IDXGIOutput* _output, DXGI_FORMAT _format)
{
	UINT count = 0;
	UINT flags = 0;

	//Call with nullptr to get list count
	_output->GetDisplayModeList(_format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	_output->GetDisplayModeList(_format, flags, &count, &modeList[0]);

	for (auto& mode : modeList)
	{
		UINT n = mode.RefreshRate.Numerator;
		UINT d = mode.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(mode.Width) + L" " +
			L"Height = " + std::to_wstring(mode.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) + L"\n";

		OutputDebugStringW(text.c_str());
		std::wcout << text << "\n";
	}
}

