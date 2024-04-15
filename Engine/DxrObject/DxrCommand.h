#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

// c++
#include <cstdint>

// ComPtr
#include <ComPtr.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace DxrObject {

	//-----------------------------------------------------------------------------------------
	// DxrObject forward
	//-----------------------------------------------------------------------------------------
	class Devices;
	class Fence;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Command class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Command {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief コンストラクタ
		//! 
		//! @param[in] devices DxrObject::Devices
		Command(Devices* device) { Init(device); }

		//! @brief デストラクタ
		~Command() { Term(); }

		//! @brief 初期化
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(Devices* device);

		//! @brief 終了処理
		void Term();

		//! @brief commandListをクローズし, Queueで実行
		void Close();

		//! @brief commandAllocator, commandListをリセット
		void Reset();


		//! @brief Signalを実行
		//! 
		//! @param[in] fences DxObject::Fence
		void Signal(Fence* fences);

		//! @brief コマンドリストを取得
		//! 
		//! @return コマンドリストを返却
		ID3D12GraphicsCommandList4* GetCommandList() const { return commandList_.Get(); }

		//! @brief コマンドキューを取得
		//! 
		//! @return コマンドキューを返却
		ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		ComPtr<ID3D12CommandQueue>         commandQueue_;
		ComPtr<ID3D12CommandAllocator>     commandAllocator_;
		ComPtr<ID3D12GraphicsCommandList4> commandList_;

	};

}