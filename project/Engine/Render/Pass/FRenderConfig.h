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
//! @brief 1回の描画に必要な設定(対象バッファ/シーン/カメラ/各種描画オプション)をまとめた構造体
struct FRenderConfig {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// IssueFlag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief config検証で検出した問題点(エラー/警告)のフラグ
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
	//! @brief 有効化する描画機能(ポストプロセス/トーンマップ等)のフラグ
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
	//! @brief 使用するアンビエントオクルージョン手法
	enum class AmbientOcclusion : uint8_t {
		None,
		XeGTAO,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GlobalIllumination enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 使用するグローバルイルミネーション手法
	enum class GlobalIllumination : uint8_t {
		None,
		Lux, //!< Lumenを参考にしたGIアルゴリズム
		ReSTIR_GI,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// AntiAliasing enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 使用するアンチエイリアス手法
	enum class AntiAliasing : uint8_t {
		None,
		FXAA,
		SMAA_1x
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// SkyVisibility enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 天空可視性(スカイライティング)の評価手法
	enum class SkyVisibility : uint8_t {
		None,
		ReSITR_Preview,
		ReSITR_Preview1,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief configの問題点(バッファ/カメラ/シーン未設定等)を検証してフラグを返す
	//! @param[in] config 検証対象のconfig
	//! @return 検出した問題点のフラグ
	static Sxl::Flag<IssueFlag> CheckIssue(const FRenderConfig& config);
	//! @brief 自身を検証し, 問題点を内部に記録する
	void CheckIssue() { issue_ = FRenderConfig::CheckIssue(*this); }

	//! @brief 指定した問題点が記録されているかを返す
	//! @param[in] flag 確認する問題点フラグ
	//! @retval true  問題あり
	//! @retval false 問題なし
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
