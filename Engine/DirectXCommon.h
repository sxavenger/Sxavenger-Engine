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
#include <DxDepthStencil.h>
#include <DxShaderBlobManager.h>

// c++
#include <memory>

// Geometry
#include <Vector4.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class WinApp;
class Texture;
class RenderTexture;

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

	//! @brief フレームの終了処理
	void EndFrame();

	/* offscreen */

	void BeginOffscreen(Texture* renderTexture);

	void EndOffscreen(Texture* renderTexture);

	void BeginOffscreens(uint32_t textureNum, RenderTexture* renderTextures[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]);

	void EndOffscreens(uint32_t textureNum, RenderTexture* renderTextures[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]);

	/* screen */

	void BeginScreenDraw();

	/* UnorderedAccess */

	void BeginUnorderedAccess(Texture* dummyTexture);

	void EndUnorderedAccess(Texture* dummyTexture);

	/* double allocator system */

	void TransitionProcess();

	void TransitionSingleAllocator();

	// ---- pipeline関係 ---- //

	// use method

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

	std::unique_ptr<DxObject::Devices>          devices_;
	std::unique_ptr<DxObject::Command>          command_;
	std::unique_ptr<DxObject::DescriptorHeaps>  descriptorHeaps_;
	std::unique_ptr<DxObject::SwapChain>        swapChains_;
	std::unique_ptr<DxObject::Fence>            fences_;
	std::unique_ptr<DxObject::ShaderBlobManager> shaderManager_;
	std::unique_ptr<DxObject::BlendState>        blendState_;
	std::unique_ptr<DxObject::DepthStencil>      depthStencil_; //!< depthStencilは共通

	/* parameters */

	bool isRendering_ = false; //!< rendering中かどうかの確認用

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BeginRendering();

	void EndRendering();
};

////////////////////////////////////////////////////////////////////////////////////////////
// static variables
////////////////////////////////////////////////////////////////////////////////////////////

static const Color4f defaultClearColor = { 0.1f, 0.25f, 0.5f, 1.0f };