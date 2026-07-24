#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"
#include "DxrBindBuffer.h"
#include "DxrExportGroup.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxDevice.h>
#include <Engine/System/DirectX/DxObject/DxCommandContext.h>
#include <Engine/System/DirectX/DxObject/DxResource.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Math/Matrix4x4.h>
#include <Lib/Sxl/Flag.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// AccelerationStructureBuffers structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 加速構造(AS)の本体バッファと構築用スクラッチバッファを保持する構造体
struct AccelerationStructureBuffers {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief ビルド入力から必要サイズを算出し, AS本体/スクラッチバッファを確保する
	//! @param[in] device DirectXデバイス
	//! @param[in] inputs AS構築の入力記述
	void Create(
		DxObject::Device* device,
		const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs
	);

	//=========================================================================================
	// public variables
	//=========================================================================================

	DxObject::Resource scratch;
	DxObject::Resource asbuffer;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateAccelerationStructureBuffer(
		DxObject::Device* device,
		const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO& info
	);

	void CreateScratchBuffer(
		DxObject::Device* device,
		const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO& info
	);

};

////////////////////////////////////////////////////////////////////////////////////////////
// BottomLevelAS class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 個々のジオメトリ(メッシュ)から構築するボトムレベル加速構造(BLAS)
class BottomLevelAS
	: protected AccelerationStructureBuffers {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BottomLevelAS()  = default;
	~BottomLevelAS() = default;

	//! @brief ジオメトリからBLASを構築する
	//! @param[in] device   DirectXデバイス
	//! @param[in] context  コマンドコンテキスト
	//! @param[in] geometry BLASの元となるジオメトリ記述
	void Build(
		DxObject::Device* device, const DxObject::CommandContext* context,
		const D3D12_RAYTRACING_GEOMETRY_DESC& geometry
	);

	//! @brief 既存のBLASを再ビルド(更新)する
	//! @param[in] context コマンドコンテキスト
	void Update(DxObject::CommandContext* context);

	//* getter *//

	//! @brief BLAS本体のGPU仮想アドレスを取得する
	//! @return GPU仮想アドレス
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return asbuffer.GetGPUVirtualAddress(); }

protected:

	//=========================================================================================
	// protected methods
	//=========================================================================================

	D3D12_RAYTRACING_GEOMETRY_DESC geometry_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// TopLevelAS class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 複数のBLASインスタンスをまとめる, レイトレのシーン全体を表すトップレベル加速構造(TLAS)
class TopLevelAS
	: protected AccelerationStructureBuffers {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Instance structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief TLASへ登録する1つのBLASインスタンス(変換行列/ID/シェーダーテーブル情報)
	struct Instance {
		//* desc書き込み
		const BottomLevelAS*                       bottomLevelAS;
		Matrix4x4                                  mat;
		UINT                                       instanceId;
		Sxl::Flag<D3D12_RAYTRACING_INSTANCE_FLAGS> flag;
		uint8_t                                    instanceMask = 0xFF;

		//* table書き込み
		std::string name; //!< exportの名前.
		WriteBindBufferDesc parameter;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TopLevelAS()  = default;
	~TopLevelAS() = default;

	//! @brief TLASを初期化する
	//! @param[in] device DirectXデバイス
	void Init(DxObject::Device* device);

	//* instance option *//

	//! @brief インスタンス登録を開始する (前フレームのインスタンスをクリアする)
	void BeginSetupInstance();

	//! @brief TLASへBLASインスタンスを1つ追加する
	//! @param[in] instance 追加するインスタンス
	void AddInstance(const Instance& instance);

	//! @brief インスタンス登録を確定し, TLASを構築(または更新)する
	//! @param[in] device  DirectXデバイス
	//! @param[in] context コマンドコンテキスト
	void EndSetupInstance(DxObject::Device* device, DxObject::CommandContext* context);

	//* getter *//

	//! @brief TLAS本体のGPU仮想アドレスを取得する
	//! @return GPU仮想アドレス
	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return asbuffer.GetGPUVirtualAddress(); }

	//! @brief 登録済みインスタンスのリストを取得する
	//! @return インスタンスリストへの参照
	const std::list<Instance>& GetInstances() const { return instances_; }

	//! @brief インスタンス記述子の数を取得する
	//! @return インスタンス数
	const size_t GetInstanceDescCount() const { return descs_->GetSize(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<Instance> instances_;
	size_t previousInstanceCount_ = 0;

	std::unique_ptr<DxObject::DimensionBuffer<D3D12_RAYTRACING_INSTANCE_DESC>> descs_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Build(DxObject::Device* device, DxObject::CommandContext* context);
	void Update(DxObject::CommandContext* context);

	bool UpdateInstanceBuffer(DxObject::Device* device);

};

DXROBJECT_NAMESPACE_END
