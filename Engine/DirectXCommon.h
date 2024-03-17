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
#include <DxBlendState.h>
#include <DxShaderBlob.h>
#include <DxDepthStencil.h>
#include <DxRootSignature.h>
#include <DxPipelineState.h>
#include <DxPipelineManager.h>

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
	void Sent();

	// ---- pipeline関係 ---- //

	void SetPipelineType(DxObject::PipelineType type) {
		pipelineManager_->SetPipelineType(type);
	}

	void SetBlendMode(BlendMode mode) {
		pipelineManager_->SetBlendMode(mode);
	}

	void SetPipelineState() {
		pipelineManager_->CreatePipeline();
		pipelineManager_->SetPipeline();
	}

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

	std::unique_ptr<DxObject::BlendState> blendState_;
	std::unique_ptr<DxObject::DepthStencil>  depthStencil_; //!< depthStencilは共通

	std::unique_ptr<DxObject::PipelineManager> pipelineManager_;

	UINT backBufferIndex_;

	//=========================================================================================
	// private methods
	//=========================================================================================
};