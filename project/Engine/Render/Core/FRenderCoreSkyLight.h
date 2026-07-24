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
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

//* lib
#include <Lib/Math/Vector2.h>
#include <Lib/Adapter/Random/Random.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreSkyLight class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 天球ライティングをReSTIRベースで解くコンピュートパイプラインを管理するRenderCore
class FRenderCoreSkyLight final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Sky ReSTIRの各処理段階に対応するパイプライン
	enum class Pipeline : uint8_t {
		InitialReservoir,
		SpatialReservoir,
		History,
		Solve,
	};
	static inline const size_t kPipelineCount = magic_enum::enum_count<Pipeline>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// Seed structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief シェーダーへ渡す乱数シード配列を保持する構造体
	//! @tparam N シード要素数
	template <size_t N>
	struct Seed {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Seed() { Set(); }

		//! @brief 全シードを一様乱数で再生成する
		void Set() { std::generate(seed.begin(), seed.end(), []() { return Random::UniformDistribution<uint32_t>(std::numeric_limits<uint32_t>::lowest(), std::numeric_limits<uint32_t>::max()); }); }

		//! @brief シード配列の先頭ポインタを取得する
		//! @return シード配列のポインタ
		uint32_t* Data() { return seed.data(); }

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::array<uint32_t, N> seed;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 全パイプラインを生成・初期化する (FBaseRenderCoreのoverride)
	void Init() override;

	//* pipeline option *//

	//! @brief 指定段階のパイプラインを設定する
	//! @param[in] pipeline 対象の処理段階
	//! @param[in] context  DirectXのキューコンテキスト
	void SetPipeline(Pipeline pipeline, const DirectXQueueContext* context) const;

	//! @brief 指定段階へコンピュート用バッファをバインドする
	//! @param[in] pipeline 対象の処理段階
	//! @param[in] context  DirectXのキューコンテキスト
	//! @param[in] desc     バインドするバッファ記述子
	void BindComputeBuffer(Pipeline pipeline, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

	//! @brief 設定中のパイプラインをディスパッチする
	//! @param[in] context    DirectXのキューコンテキスト
	//! @param[in] resolution 処理対象の解像度
	void Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* compute pipeline *//

	std::array<CustomReflectionComputePipeline, kPipelineCount> pipelines_ = {};

	static inline const Vector3ui kThreadGroupSize = { 16, 16, 1 };

	//* directory *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Sky";

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();

	//* helper methods *//

	void CreateComputePipeline(Pipeline pipeline, std::filesystem::path compute);

};

SXAVENGER_ENGINE_NAMESPACE_END
