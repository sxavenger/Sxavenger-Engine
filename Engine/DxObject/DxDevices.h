#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

// c++
#include <cstdint>
#include <cassert>

// ComPtr
#include <ComPtr.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Devices class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Devices {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief コンストラクタ
		Devices();

		//! @brief デストラクタ
		~Devices();

		//! @brief 初期化
		void Init();

		//! @brief 終了処理
		void Term();

		//! @brief デバイスを取得
		//! 
		//! @return デバイスを返却
		ID3D12Device* GetDevice() const { return device_.Get(); }

		//! @brief DXGIファクトリーを取得
		//! 
		//! @return DXGIファクトリーを返却
		IDXGIFactory7* GetFactory() const { return dxgiFactory_.Get(); }

	private:

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

		//=========================================================================================
		// private variables
		//=========================================================================================

		DxLeakChecker debugLeak;
		ComPtr<ID3D12Debug1> debugController_;

		ComPtr<IDXGIFactory7> dxgiFactory_;
		ComPtr<IDXGIAdapter4> useAdapter_;

		ComPtr<ID3D12Device> device_;
	};
}