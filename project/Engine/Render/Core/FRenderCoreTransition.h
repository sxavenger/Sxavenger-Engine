#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderCore.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>
#include <Engine/Module/Pipeline/CustomGraphicsPipeline.h>

//* c++
#include <array>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreTransition class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief バッファ間の中間変換処理と最終presentのパイプラインを管理するRenderCore
class FRenderCoreTransition final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transition enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 中間バッファ変換処理の種別
	enum class Transition : uint8_t {
		MotionVectorTransition,
		AlbedoWhiteTransition,
		LightingTransition,
		ReflectionTransition,
		MipmapTransition,
		TransparentTransition,
	};
	static inline const size_t kTransitionCount = magic_enum::enum_count<Transition>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 全パイプラインを生成・初期化する (FBaseRenderCoreのoverride)
	void Init() override;

	//* transition option *//

	//! @brief 指定変換処理のパイプラインを設定する
	//! @param[in] transition 対象の変換処理
	//! @param[in] context    DirectXのキューコンテキスト
	void SetPipeline(Transition transition, const DirectXQueueContext* context) const;

	//! @brief 指定変換処理へコンピュート用バッファをバインドする
	//! @param[in] transition 対象の変換処理
	//! @param[in] context    DirectXのキューコンテキスト
	//! @param[in] desc       バインドするバッファ記述子
	void BindComputeBuffer(
		Transition transition, const DirectXQueueContext* context,
		const DxObject::BindBufferDesc& desc
	) const;

	//! @brief 設定中の変換処理をディスパッチする
	//! @param[in] context    DirectXのキューコンテキスト
	//! @param[in] resolution 処理対象の解像度
	void Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const;

	//* present option *//

	//! @brief 指定テクスチャを最終出力(スワップチェーン)へ描画する
	//! @param[in] context    DirectXのキューコンテキスト
	//! @param[in] resolution 出力解像度
	//! @param[in] handle     表示するテクスチャのSRV GPUハンドル
	void Present(const DirectXQueueContext* context, const Vector2ui& resolution, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline const Vector2ui kNumthread             = { 16, 16 };
	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Transition";

	std::array<CustomReflectionComputePipeline, kTransitionCount> transitions_;
	DxObject::ReflectionGraphicsPipelineState presenter_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePresent();

	void CreateTransition(Transition transition, const std::filesystem::path& filepath);

};

SXAVENGER_ENGINE_NAMESPACE_END
