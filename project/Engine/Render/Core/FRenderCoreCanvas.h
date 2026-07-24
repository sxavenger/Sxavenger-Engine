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
#include <Engine/Module/Pipeline/CustomGraphicsPipeline.h>

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
// FRenderCoreCanvas class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief スプライト/テキストなど2D Canvas描画用のパイプラインを管理するRenderCore
class FRenderCoreCanvas final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Canvas描画パイプラインの種別
	enum class Pipeline : uint8_t {
		Sprite,
		Text
	};
	static const size_t kPipelineCount = magic_enum::enum_count<Pipeline>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 全パイプラインを生成・初期化する (FBaseRenderCoreのoverride)
	void Init() override;

	//* pipeline option *//

	//! @brief 指定パイプラインをコマンドリストへ設定する
	//! @param[in] pipeline   設定するパイプライン
	//! @param[in] context    DirectXのキューコンテキスト
	//! @param[in] resolution ビューポート解像度
	void SetPipeline(Pipeline pipeline, const DirectXQueueContext* context, const Vector2ui& resolution) const;

	//! @brief 指定パイプラインへ描画用バッファをバインドする
	//! @param[in] pipeline 対象パイプライン
	//! @param[in] context  DirectXのキューコンテキスト
	//! @param[in] desc     バインドするバッファ記述子
	void BindGraphicsBuffer(Pipeline pipeline, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* graphics pipeline *//

	std::array<CustomReflectionGraphicsPipeline, kPipelineCount> pipelines_;
	DxObject::GraphicsPipelineDesc desc_ = {};

	//* directory *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Canvas";

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDesc();

	void CreatePipeline();

	//* helper methods *//

	void CreatePipeline(
		Pipeline pipeline,
		std::filesystem::path vertex, std::filesystem::path pixel
	);

};

SXAVENGER_ENGINE_NAMESPACE_END
