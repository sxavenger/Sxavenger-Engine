#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <Engine/System/DxObject/DxDevices.h>
#include <Engine/System/DxObject/DxCommand.h>
#include <Engine/System/DxObject/DxDescriptorHeaps.h>
#include <Engine/System/DxObject/DxSwapChain.h>
#include <Engine/System/DxObject/DxFence.h>
#include <Engine/System/DxObject/DxBlendState.h>
#include <Engine/System/DxObject/DxDepthStencil.h>
#include <Engine/System/DxObject/DxShaderBlobManager.h>

// c++
#include <memory>

// Geometry
#include <Lib/Geometry/Vector4.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class WinApp;

//* Texture
class Texture;
class RenderTexture;
class DummyTexture;

//* DepthRender
class DepthRenderTarget;

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXCommon class
////////////////////////////////////////////////////////////////////////////////////////////
class DirectXCommon {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	DirectXCommon() = default;

	//! @brief デストラクタ
	~DirectXCommon() = default;

	//! @brief DirectXの初期化
	void Init(WinApp* winApp);

	//! @brief DirectXの終了
	void Term();

	//! @brief フレームの終了処理
	void EndFrame();

	/* offscreen */

	//* RenderTexture

	void BeginOffscreen(RenderTexture* renderTexture);

	void EndOffscreen(RenderTexture* renderTexture);

	void BeginOffscreens(uint32_t textureNum, RenderTexture* renderTextures[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]);

	void EndOffscreens(uint32_t textureNum, RenderTexture* renderTextures[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]);

	//* DepthRenderTarget

	void BeginOffScreen(DepthRenderTarget* depthRenderTarget, bool isClearRenderTarget = false);

	void EndOffScreen(DepthRenderTarget* depthRenderTarget);


	/* screen */

	void BeginScreenDraw();

	/* UnorderedAccess */

	void BeginUnorderedAccess(DummyTexture* dummyTexture);

	void EndUnorderedAccess(DummyTexture* dummyTexture);

	/* double allocator system */

	void TranstionAllocator();

	void ResetBackAllocator();


	// ---- pipeline関係 ---- //

	// use method

	// ---- resource関係 ---- //

	void CopyResource(
		ID3D12Resource* dst, D3D12_RESOURCE_STATES dstState,
		ID3D12Resource* src, D3D12_RESOURCE_STATES srcState
	);

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