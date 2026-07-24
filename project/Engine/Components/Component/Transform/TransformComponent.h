#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Math/Matrix4x4.h>
#include <Lib/Transform/Transformation.h>
#include <Lib/Transform/TransformationMatrix.h>
#include <Lib/Transform/TransformQuaternion.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 3D空間の変換情報component.
class TransformComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TransformComponent(EntityBehaviour* behaviour);
	~TransformComponent() override = default;

	//* component option *//

	//! @brief インスペクタにこのcomponentの編集UIを表示する (BaseComponentのoverride)
	void ShowComponentInspector() override;

	//* buffer option *//

	//! @brief 変換行列定数バッファのGPU仮想アドレスを取得する
	//! @return 定数バッファのGPU仮想アドレス
	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	//* transform option *//

	//! @brief 行列を更新
	void UpdateMatrix();

	//! @brief 親が存在するか確認
	bool HasParent() const;

	//! @brief 保持しているクォータニオン変換を取得する
	//! @return TransformQuaternionへの参照
	const TransformQuaternion& GetTransform() const { return transform_; }
	//! @brief 保持しているクォータニオン変換を取得する (非const版)
	//! @return TransformQuaternionへの参照
	TransformQuaternion& GetTransform() { return transform_; }

	//! @brief SRT成分をまとめた変換情報を取得する
	//! @return Transformationへの参照
	const Transformation& GetTransformation() const { return transformation_; }

	//! @brief 親の変換を合成した変換行列情報を取得する
	//! @return TransformationMatrixへの参照
	const TransformationMatrix& GetTransformationMatrix() const;

	//! @brief ワールド変換行列を取得する
	//! @return ワールド行列への参照
	const Matrix4x4& GetMatrix() const;

	//! @brief ワールド座標の位置を取得する
	//! @return ワールド位置
	const Vector3f GetPosition() const;

	//! @brief ワールド空間での前方向ベクトルを取得する
	//! @return 前方向ベクトル
	const Vector3f GetDirection() const;

	//* operator [accessor] *//

	//! @brief 内部のTransformQuaternionへアクセスする (アロー演算子)
	const TransformQuaternion* operator->() const { return &transform_; }
	//! @brief 内部のTransformQuaternionへアクセスする (アロー演算子, 非const版)
	TransformQuaternion* operator->() { return &transform_; }

	//! @brief 内部のTransformQuaternionを参照する (間接演算子)
	const TransformQuaternion& operator*() const { return transform_; }
	//! @brief 内部のTransformQuaternionを参照する (間接演算子, 非const版)
	TransformQuaternion& operator*() { return transform_; }

	//* json option *//

	//! @brief このcomponentの状態をjsonへシリアライズする (BaseComponentのoverride)
	//! @return シリアライズしたjson
	json ParseToJson() const override;

	//! @brief jsonからこのcomponentの状態を復元する (BaseComponentのoverride)
	//! @param[in] data 入力元のjson
	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* transformation *//

	TransformQuaternion transform_ = {};
	Transformation transformation_ = {};

	//* buffer *//

	std::unique_ptr<DxObject::ConstantBuffer<TransformationMatrix>> buffer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void TransferGPU();

};

SXAVENGER_ENGINE_NAMESPACE_END
