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
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Device class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief DirectXのDeviceを管理するクラス.
class Device {
public:

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Device() = default;
	~Device() { Term(); }

	void Init();

	void Term();

	//* option *//

	void CheckDeviceStatus() const;

	//* getter *//

	ID3D12Device8* GetDevice() const { return device_.Get(); }

	IDXGIFactory7* GetFactory() const { return dxgiFactory_.Get(); }

	IDXGIAdapter4* GetAdapter() const { return useAdapter_.Get(); }

	//* feature getter *//

	D3D_SHADER_MODEL GetShaderModelTier() const { return shaderModelTier_; }

	D3D12_RAYTRACING_TIER GetRaytracingTier() const { return raytracingTier_; }

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

	//* feature member *//

	D3D_SHADER_MODEL shaderModelTier_;
	D3D12_RAYTRACING_TIER raytracingTier_;
	bool isMeshShaderSupport_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDebugLayer();
	void CreateFactory();
	void CreateAdapter();
	void CreateDevice();
	void CreateInfoQueue();

	D3D_SHADER_MODEL CheckShaderModelTier() const;
	D3D12_RAYTRACING_TIER CheckRaytracingTier() const;
	bool CheckMeshShaderEnable() const;

};

DXOBJECT_NAMESPACE_END
