#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderCore.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

//* lib
#include <Lib/Math/Vector2.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <array>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreProcess class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ポストプロセス/合成処理(トーンマップ/AA/GTAO/Bloom等)のコンピュートパイプラインを管理するRenderCore
class FRenderCoreProcess final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// CompositeProcess enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 合成段階で実行する処理(トーンマップ/アンチエイリアス/AO)の種別
	enum class CompositeProcess : uint8_t {
		Tonemap,
		FXAA,
		SMAA_EdgeDetection,
		SMAA_BlendWeight,
		SMAA_NeighborhoodBlending,
		XeGTAO_PrefilterDepth,
		XeGTAO_Main,
		XeGTAO_DenoiseFirst,
		XeGTAO_DenoiseLast,
		XeGTAO_Resolve,
	};
	static const size_t	kCompositeProcessCount = magic_enum::enum_count<CompositeProcess>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// PostProcess enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ユーザー指定で適用するポストエフェクトの種別
	enum class PostProcess : uint32_t {
		GrayScale,
		Bloom_Luminance,
		Bloom_Apply,
		AutoExposure_Luminance,
		AutoExposure_Average,
		AutoExposure_Apply,
		LocalExposure,
		RadialBlur,
		MotionBlur,
		ChromaticAberration,
		Vignette,
		LUT_Convert,
		LUT,
		PostFxBrightness,
		PostFxSaturation,
		PostFxColorfulness,
		Posterize,
		Sketch,
	};
	static const size_t kPostProcessCount = magic_enum::enum_count<PostProcess>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 全パイプラインを生成・初期化する (FBaseRenderCoreのoverride)
	void Init() override;

	//* process option *//

	//! @brief 現在設定中のコンピュートパイプラインをディスパッチする
	//! @param[in] context    DirectXのキューコンテキスト
	//! @param[in] resolution 処理対象の解像度 (スレッドグループ数の算出に使用)
	void Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const;

	//* post process option *//

	//! @brief 指定ポストエフェクトのパイプラインを設定する
	//! @param[in] process 対象のポストエフェクト
	//! @param[in] context DirectXのキューコンテキスト
	void SetPipeline(PostProcess process, const DirectXQueueContext* context) const;

	//! @brief 指定ポストエフェクトへコンピュート用バッファをバインドする
	//! @param[in] process 対象のポストエフェクト
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] desc    バインドするバッファ記述子
	void BindComputeBuffer(PostProcess process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

	//* composite process option *//

	//! @brief 指定合成処理のパイプラインを設定する
	//! @param[in] process 対象の合成処理
	//! @param[in] context DirectXのキューコンテキスト
	void SetPipeline(CompositeProcess process, const DirectXQueueContext* context) const;

	//! @brief 指定合成処理へコンピュート用バッファをバインドする
	//! @param[in] process 対象の合成処理
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] desc    バインドするバッファ記述子
	void BindComputeBuffer(CompositeProcess process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* process pipelines *//

	std::array<CustomReflectionComputePipeline, kPostProcessCount> post_;
	std::array<CustomReflectionComputePipeline, kCompositeProcessCount> composite_;

	// TODO: Shader Script への移行

	//* process constants *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Process";
	static inline const Vector2ui kNumthreads = { 16, 16 };

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();

	//* helper methods *//

	void CreatePipeline(PostProcess process, const std::filesystem::path& filepath);
	void CreatePipeline(PostProcess process, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc);

	void CreatePipeline(CompositeProcess process, const std::filesystem::path& filepath);
	void CreatePipeline(CompositeProcess process, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc);

};

SXAVENGER_ENGINE_NAMESPACE_END
