#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"

//* DirectX12
#include <dxgidebug.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dxguid.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Device class
////////////////////////////////////////////////////////////////////////////////////////////
class Device {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Device() = default;
	~Device() { Term(); }

	void Init();

	void Term();

	ID3D12Device8* GetDevice() const { return device_.Get(); }

	IDXGIFactory7* GetFactory() const { return dxgiFactory_.Get(); }

public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// DxLeakChecker class
	////////////////////////////////////////////////////////////////////////////////////////////
	class DxLeakChecker {
	public:

		~DxLeakChecker() {
			ComPtr<IDXGIDebug1> debug;

			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
				debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
				debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
				debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			}
		}
	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX *//

	ComPtr<ID3D12Debug1> debugController_;
	ComPtr<ID3D12InfoQueue> infoQueue_;

	ComPtr<IDXGIFactory7> dxgiFactory_;
	ComPtr<IDXGIAdapter4> useAdapter_;

	ComPtr<ID3D12Device8> device_;

	//* member *//

	bool isMeshShaderEnabled_;
	bool isRayTracingEnabled_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDebugLayer();
	void CreateFactory();
	void CreateAdapter();
	void CreateDevice();
	void CreateInfoQueue();

	bool CheckShaderModel();
	bool CheckRayTracingEnable();
	bool CheckMeshShaderEnable();

	bool CheckLaunchFromPIX();

};

_DXOBJECT_NAMESPACE_END
