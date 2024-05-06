#pragma once
#include <d3d11.h>
#include <vector>
#include <wrl/client.h>
#include "Engine/Utilities/ErrorLogger.h"

class AdapterData
{
public:
    AdapterData(IDXGIAdapter* pAdapter);
    IDXGIAdapter* pAdapter = nullptr;
    DXGI_ADAPTER_DESC description;
};

class Adapters
{
public:
    static std::vector<AdapterData> GetAdapters();

private:
    static std::vector<AdapterData> adapters;
};
