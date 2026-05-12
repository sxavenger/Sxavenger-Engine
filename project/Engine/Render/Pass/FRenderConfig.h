#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FRenderTargetBuffer.h"
#include "../Scene/FScene.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxSwapChain.h>
#include <Engine/Components/Component/Camera/CameraComponent.h>

//* lib
#include <Lib/Sxl/Flag.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderConfig structure
////////////////////////////////////////////////////////////////////////////////////////////
struct FRenderConfig {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// IssueFlag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class IssueFlag : size_t {
		Success = 0,

		//* critical error *//

		Error_Buffer = 1 << 0,

		//* warning *//

		Warning_Camera = 1 << 1,
		Warning_Scene  = 1 << 2,

		//* issue group *//

		Warning_Geometry = Warning_Camera | Warning_Scene, //!< Geometryの描画に問題がある.

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// OptionFlag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class OptionFlag : size_t {
		None        = 0,
		PostProcess = 1 << 0,
		Tonemap     = 1 << 1,

		LightingOnly = 1 << 2,

		//* option group *//

		Default = PostProcess | Tonemap,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// AmbientOcclusion enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class AmbientOcclusion : uint8_t {
		None,
		XeGTAO,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GlobalIllumination enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class GlobalIllumination : uint8_t {
		None,
		Lux, //!< Lumenを参考にしたGIアルゴリズム
		ReSTIR_GI,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// AntiAliasing enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class AntiAliasing : uint8_t {
		None,
		FXAA,
		SMAA_1x
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// SkyVisibility enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class SkyVisibility : uint8_t {
		None,
		ReSITR_Preview,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static Sxl::Flag<IssueFlag> CheckIssue(const FRenderConfig& config);
	void CheckIssue() { issue_ = FRenderConfig::CheckIssue(*this); }

	bool HasIssue(IssueFlag flag) const { return issue_.Any(flag); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* parameter *//

	std::string name; //!< 描画の識別名

	//* render target buffer *//

	FRenderTargetBuffer* buffer = nullptr; //!< 描画対象のバッファ

	//* color space *//

	DxObject::SwapChain::ColorSpace colorSpace = DxObject::SwapChain::ColorSpace::Rec_709;
	//!< 描画対象のカラースペース

	//* scene *//

	FScene* scene = nullptr; //!< 描画対象のシーン

	//* camera setting *//

	const CameraComponent* camera = nullptr;
	CameraComponent::Tag tag      = CameraComponent::Tag::Game;
	//!< cameraが直接指定されていない場合, tagのcameraを使用する

	const CameraComponent* cullCamera = nullptr; //!< カリング用camera
	//!< カリング用cameraが直接指定されていない場合, 設定されたcameraを使用する

	//* option *//

	Sxl::Flag<OptionFlag> option = OptionFlag::Default;

	AmbientOcclusion ambientOcclusion     = AmbientOcclusion::XeGTAO;
	GlobalIllumination globalIllumination = GlobalIllumination::None;
	AntiAliasing antiAliasing             = AntiAliasing::FXAA;
	SkyVisibility skyVisibility           = SkyVisibility::None;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Sxl::Flag<IssueFlag> issue_ = IssueFlag::Success; //!< configの問題点

};

SXAVENGER_ENGINE_NAMESPACE_END
