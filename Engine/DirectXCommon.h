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
#include <DxCompilers.h>
#include <DxShaderBlob.h>
#include <DxShaderTable.h>
#include <DxDepthStencil.h>
#include <DxRootSignature.h>
#include <DxPipelineState.h>
#include <DxPipelineManager.h>

// DxSource
#include <DxCSBlob.h>

// c++
#include <memory>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class WinApp;
class Texture;

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

	void BeginOffscreen(Texture* dummyTexture);
	void EndOffscreen();

	void BeginScreenDraw();

	void TransitionProcess();

	void TransitionSingleAllocator();

	// ---- pipeline関係 ---- //

	void SetPipelineType(PipelineType type) {
		pipelineManager_->SetPipelineType(type);
	}

	void SetBlendMode(BlendMode mode) {
		pipelineManager_->SetBlendMode(mode);
	}

	void SetPipelineState() {
		pipelineManager_->CreatePipeline();
		pipelineManager_->SetPipeline();
	}

	// ---- resource関係 ---- //

	void CopyResource(
		ID3D12Resource* dst, D3D12_RESOURCE_STATES dstState,
		ID3D12Resource* src, D3D12_RESOURCE_STATES srcState
	);

	static DirectXCommon* GetInstance();

	// TODO: コマンドリストを外に出したくない...
	auto GetCommandList() const { return command_->GetCommandList(); }

	DxObject::Devices* GetDeviceObj() const { return devices_.get(); }
	DxObject::DescriptorHeaps* GetDescriptorsObj() const { return descriptorHeaps_.get(); }
	DxObject::SwapChain* GetSwapChainObj() const { return swapChains_.get(); } //!< ImGuiManagerで使う kBufferCount

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	DxObject::Devices::DxLeakChecker leakChecher_;

	std::unique_ptr<DxObject::Devices>         devices_;
	std::unique_ptr<DxObject::Command>         command_;
	std::unique_ptr<DxObject::DescriptorHeaps> descriptorHeaps_;
	std::unique_ptr<DxObject::SwapChain>       swapChains_;
	std::unique_ptr<DxObject::Fence>           fences_;
	std::unique_ptr<DxObject::Compilers>       compilers_;
	std::unique_ptr<DxObject::ShaderTable>     shaderTable_;

	std::unique_ptr<DxObject::BlendState> blendState_;
	std::unique_ptr<DxObject::DepthStencil>  depthStencil_; //!< depthStencilは共通

	std::unique_ptr<DxObject::PipelineManager> pipelineManager_;

	UINT backBufferIndex_;

	Texture* offscreenDummyTexture_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================
};