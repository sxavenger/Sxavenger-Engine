#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

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

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject {

	//-----------------------------------------------------------------------------------------
	// DxObject forward
	//-----------------------------------------------------------------------------------------
	class Devices;
	class Command;
	class ShaderBlob;
	class RootSignature;

	////////////////////////////////////////////////////////////////////////////////////////////
	// PipelineState class
	////////////////////////////////////////////////////////////////////////////////////////////
	class PipelineState {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief コンストラクタ
		//! 
		//! @param[in] devices      DxObject::Device
		//! @param[in] shaderBlob   DxObject::Shader
		//! @param[in] clientWidth  クライアント領域横幅
		//! @param[in] clientHeight クライアント領域縦幅
		PipelineState(
			Devices* devices, ShaderBlob* shaderBlob, RootSignature* rootSignature,
			int32_t clientWidth, int32_t clientHeight
		);

		//! @brief デストラクタ
		~PipelineState();

		//! @brief 初期化処理
		//! 
		//! @param[in] devices      DxObject::Device
		//! @param[in] shaderBlob   DxObject::Shader
		//! @param[in] clientWidth  クライアント領域横幅
		//! @param[in] clientHeight クライアント領域縦幅
		void Init(
			Devices* devices, ShaderBlob* shaderBlob, RootSignature* rootSignature,
			int32_t clientWidth, int32_t clientHeight
		);

		//! @brief 終了処理
		void Term();

		//! @brief commandListにPipelineStateを設定
		void SetCommandPipelineState(Command* command);

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		ID3D12RootSignature* rootSignature_;

		ComPtr<ID3D12PipelineState> graphicsPipelineState_;

		D3D12_VIEWPORT viewport_;
		D3D12_RECT     scissorRect_;

	};

}