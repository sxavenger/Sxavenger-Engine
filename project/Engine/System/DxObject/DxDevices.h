#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include "DxObjectCommon.h"

//* DirectX12
#include <dxgidebug.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dxguid.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Devices class
////////////////////////////////////////////////////////////////////////////////////////////
class Devices {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	Devices() { Init(); }

	//! @brief デストラクタ
	~Devices() { Term(); }

	//! @brief 初期化
	void Init();

	//! @brief 終了処理
	void Term();

	//! @brief デバイスを取得
	//! 
	//! @return デバイスを返却
	auto const GetDevice() const { return device_.Get(); }

	//! @brief DXGIファクトリーを取得
	//! 
	//! @return DXGIファクトリーを返却
	auto const GetFactory() const { return dxgiFactory_.Get(); }

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

	ComPtr<ID3D12Debug1> debugController_;
	ComPtr<ID3D12InfoQueue> infoQueue_;

	ComPtr<IDXGIFactory7> dxgiFactory_;
	ComPtr<IDXGIAdapter4> useAdapter_;

	ComPtr<ID3D12Device8> device_;

	//* member *//

	bool isMeshShaderEnabled_;
	bool isRayTracingEnabled_;

	//* config *//

	bool useDebugLayer_ = true;
	//!< debug時のみ有効
	//!< PIX起動時は自動的にoffに

	//=========================================================================================
	// private methods
	//=========================================================================================

	bool CheckRunningPIX();

	bool CheckRayTracingEnable();
	bool CheckMeshShaderEnable();

};

_DXOBJECT_NAMESPACE_END