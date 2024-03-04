#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxDevices.h>
#include <DxCommand.h>
#include <DxDescriptorHeaps.h>
#include <DxSwapChain.h>
#include <DxFence.h>
#include <DxShaderBlob.h>
#include <DxDepthStencil.h>
#include <DxRootSignature.h>
#include <DxPipelineState.h>

// c++
#include <memory>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class WinApp;

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXCommon class
////////////////////////////////////////////////////////////////////////////////////////////
class DirectXCommon {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	DirectXCommon();

	//! @brief デストラクタ
	~DirectXCommon();

	//! @brief DirectXの初期化
	void Init(WinApp* winApp, int32_t clientWidth, int32_t clientHeight);

	//! @brief DirectXの終了
	void Term();

	//! @brief フレームの開始処理
	void BeginFrame();

	//! @brief フレームの終了処理
	void EndFrame();

	//!
	void End();

	void SetPipelineState(uint32_t index);

	static DirectXCommon* GetInstance();

	// TODO: コマンドリストを外に出したくない...
	ID3D12GraphicsCommandList* GetCommandList() const { return command_->GetCommandList(); }
	DxObject::Devices* GetDeviceObj() const { return devices_.get(); }

	DxObject::DescriptorHeaps* GetDescriptorsObj() const { return descriptorHeaps_.get(); }
	DxObject::SwapChain* GetSwapChainObj() const { return swapChains_.get(); } //!< ImGuiManagerで使う kBufferCount

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::Devices>         devices_;
	std::unique_ptr<DxObject::Command>         command_;
	std::unique_ptr<DxObject::DescriptorHeaps> descriptorHeaps_;
	std::unique_ptr<DxObject::SwapChain>       swapChains_;
	std::unique_ptr<DxObject::Fence>           fences_;

	static const uint32_t kPipelineNum_ = 2;

	std::unique_ptr<DxObject::DepthStencil>  depthStencil_; //!< depthStencilは共通
	std::unique_ptr<DxObject::ShaderBlob>    shaderBlob_[2];
	std::unique_ptr<DxObject::RootSignature> rootSignature_[2];
	std::unique_ptr<DxObject::PipelineState> pipeline_[2];

	UINT backBufferIndex_;

	//=========================================================================================
	// private methods
	//=========================================================================================
};