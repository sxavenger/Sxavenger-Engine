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
// FRenderCoreGeometry class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ジオメトリ描画(GBuffer書き込み/前段/透過)用のグラフィックスパイプラインを管理するRenderCore
class FRenderCoreGeometry final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 描画方式(Deferred/Forward)と頂点処理(VS/MeshShader)の組み合わせパイプライン
	enum class Pipeline : uint32_t {
		ForwardPrepass_MeshVS,
		ForwardPrepass_MeshMS,
		ForwardTransparent_MeshVS,
		ForwardTransparent_MeshMS,
		Deferred_MeshVS,
		Deferred_MeshMS,
	};
	static inline const size_t kPipelineCount = magic_enum::enum_count<Pipeline>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// Desc enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief パイプライン生成に用いる出力先ごとのPSO記述子種別
	enum class Desc : uint8_t {
		Deferred,
		ForwardPrepass,
		ForwardTransparent,
	};
	static inline const size_t kDescCount = magic_enum::enum_count<Desc>();

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

	//! @brief 指定パイプラインへ描画用バッファ(定数/リソース)をバインドする
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
	std::array<DxObject::GraphicsPipelineDesc, kDescCount> descs_ = {};

	// TODO: Shader Script への移行

	//* directory *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Geometry";

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDesc();

	void CreatePipeline();

};

SXAVENGER_ENGINE_NAMESPACE_END
