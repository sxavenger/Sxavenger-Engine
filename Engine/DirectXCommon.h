#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <ObjectPtr.h>

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

	void SetPipelineState(uint32_t index);

	static DirectXCommon* GetInstance();

	// TODO: コマンドリストを外に出したくない...
	ID3D12GraphicsCommandList* GetCommandList() const { return command_->GetCommandList(); }
	DxObject::Devices* GetDeviceObj() const { return devices_.Get(); }

	DxObject::DescriptorHeaps* GetDescriptorsObj() const { return descriptorHeaps_.Get(); }
	DxObject::SwapChain* GetSwapChainObj() const { return swapChains_.Get(); } //!< ImGuiManagerで使う kBufferCount

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Object_Ptr<DxObject::Devices>          devices_;
	Object_Ptr<DxObject::Command>         command_;
	Object_Ptr<DxObject::DescriptorHeaps> descriptorHeaps_;
	Object_Ptr<DxObject::SwapChain>       swapChains_;
	Object_Ptr<DxObject::Fence>           fences_;

	static const uint32_t kPipelineNum_ = 2;

	Object_Ptr<DxObject::DepthStencil>  depthStencil_; //!< depthStencilは共通
	Object_Ptr<DxObject::ShaderBlob>    shaderBlob_[2];
	Object_Ptr<DxObject::RootSignature> rootSignature_[2];
	Object_Ptr<DxObject::PipelineState> pipeline_[2];

	UINT backBufferIndex_;

	//=========================================================================================
	// private methods
	//=========================================================================================
};