#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderCore.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/DirectX/DxrObject/DxrExportGroup.h>
#include <Engine/System/DirectX/DxrObject/DxrRaytracingBlob.h>
#include <Engine/System/DirectX/DxrObject/DxrStateObjectContext.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

//* externals
#include <magic_enum.hpp>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreReSTIR class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief レイトレーシング + ReSTIRによる大域照明のシェーダーテーブルとパイプラインを管理するRenderCore
class FRenderCoreReSTIR final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RaygenerationExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief RayGenerationシェーダーのエクスポート種別
	enum class RaygenerationExportType : uint32_t {
		Default,
	};
	static const size_t kRaygenerationExportCount = magic_enum::enum_count<RaygenerationExportType>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// MissExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Missシェーダーのエクスポート種別
	enum class MissExportType : uint32_t {
		Default,
	};
	static const size_t kMissExportCount = magic_enum::enum_count<MissExportType>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// HitgroupExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief HitGroupシェーダーのエクスポート種別
	enum class HitgroupExportType : uint32_t {
		Mesh,
		Emissive,
	};
	static const size_t kHitgroupExportCount = magic_enum::enum_count<HitgroupExportType>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// Setting structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ReSTIRのサンプル数設定を保持する構造体
	struct Setting {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t maxSampleCount  = 1024; //!< 合計sample数
		uint32_t samplesPerFrame = 1;    //!< frameごとのsample数

	};

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

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::array<uint32_t, N> seed;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Process enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ReSTIRの処理段階 (リセット/時間再利用/解決)
	enum class Process : uint8_t {
		Reset,
		Temporal,
		Solve,
	};
	static const size_t kProcessCount = magic_enum::enum_count<Process>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief レイトレ用エクスポートグループとパイプラインを生成・初期化する (FBaseRenderCoreのoverride)
	void Init() override;

	//* export option *//

	//! @brief 指定種別のRayGenerationエクスポートグループを取得する
	//! @param[in] raygeneration 対象種別
	//! @return エクスポートグループへのポインタ
	const DxrObject::ExportGroup* GetExportGroup(RaygenerationExportType raygeneration) const;

	//! @brief 指定種別のMissエクスポートグループを取得する
	//! @param[in] miss 対象種別
	//! @return エクスポートグループへのポインタ
	const DxrObject::ExportGroup* GetExportGroup(MissExportType miss) const;

	//! @brief 指定種別のHitGroupエクスポートグループを取得する
	//! @param[in] hitgroup 対象種別
	//! @return エクスポートグループへのポインタ
	const DxrObject::ExportGroup* GetExportGroup(HitgroupExportType hitgroup) const;

	//* context option *//

	//! @brief TLASに合わせてシェーダーテーブルを更新する
	//! @param[in] topLevelAS トップレベル加速構造
	void UpdateShaderTable(const DxrObject::TopLevelAS* topLevelAS);

	//! @brief レイトレ用ステートオブジェクトコンテキストを取得する
	//! @return コンテキストへのポインタ
	DxrObject::StateObjectContext* GetContext() const { return context_.get(); }

	//* pipeline option *//

	//! @brief 指定処理段階のパイプラインを設定する
	//! @param[in] process 対象の処理段階
	//! @param[in] context DirectXのキューコンテキスト
	void SetPipeline(Process process, const DirectXQueueContext* context) const;

	//! @brief 指定処理段階へコンピュート用バッファをバインドする
	//! @param[in] process 対象の処理段階
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] desc    バインドするバッファ記述子
	void BindComputeBuffer(Process process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

	//! @brief 設定中のパイプラインをディスパッチする
	//! @param[in] context    DirectXのキューコンテキスト
	//! @param[in] resolution 処理対象の解像度
	void Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directory *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "ReSTIR";

	//* DirectX Raytracing *//

	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kRaygenerationExportCount> raygenerationExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kMissExportCount>          missExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kHitgroupExportCount>      hitgroupExportGroups_;

	std::unique_ptr<DxrObject::StateObjectContext> context_;

	//* pipeline *//

	std::array<CustomReflectionComputePipeline, kProcessCount> pipelines_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* create helper methods *//

	void CreateRaygeneration();

	void CreateMiss();

	void CreateHitgroup();

	void CreateContext();

	void CreatePipeline();

	void CreateComputePipeline(Process process, const std::filesystem::path& filepath);

};

SXAVENGER_ENGINE_NAMESPACE_END
