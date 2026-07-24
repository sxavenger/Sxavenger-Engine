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
// FRenderCoreDecal class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief デカール(投影テクスチャ)描画用のパイプラインを管理するRenderCore
class FRenderCoreDecal final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Desc enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief PSO記述子の種別
	enum class Desc : uint8_t {
		Decal,
	};
	static inline const size_t kDescCount = magic_enum::enum_count<Desc>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief デカール描画パイプラインの種別
	enum class Pipeline : uint32_t {
		Decal,
	};
	static inline const size_t kPipelineCount = magic_enum::enum_count<Pipeline>();

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

	//! @brief デカール描画のドローコールを発行する
	//! @param[in] context DirectXのキューコンテキスト
	void DrawCall(const DirectXQueueContext* context);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* graphics pipeline *//

	std::array<CustomReflectionGraphicsPipeline, kPipelineCount> pipelines_;
	std::array<DxObject::GraphicsPipelineDesc, kDescCount> descs_ = {};

	// TODO: Shader Script への移行

	//* directory *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Decal";

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDesc();

	void CreatePipeline();

};

SXAVENGER_ENGINE_NAMESPACE_END
