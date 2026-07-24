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
#include <Lib/Math/Vector2.h>
#include <Lib/Math/Matrix4x4.h>
#include <Lib/Transform/Transformation.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// CameraComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ビュー/プロジェクション行列を管理し, GPUへ転送するカメラcomponent
class CameraComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Tag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief カメラの用途タグ (ゲーム用/エディタ用)
	enum class Tag : uint8_t {
		None,
		Game,
		Editor,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Camera structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief GPUへ転送するカメラ行列一式(ビュー/ワールド/プロジェクション等)を保持する構造体
	struct Camera {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief 各行列を単位行列等で初期化する
		void Init();

		//! @brief 変換情報からビュー/ワールド行列を更新する
		//! @param[in] _transformation カメラのワールド変換
		void TransferView(const Transformation& _transformation);

		//! @brief プロジェクション行列と, その逆行列を更新する
		//! @param[in] _proj プロジェクション行列
		void TransferProj(const Matrix4x4& _proj);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Matrix4x4 view;
		Matrix4x4 world;
		Matrix4x4 proj;
		Matrix4x4 projInv;
		float nearZ;
		float farZ;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Projection structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 物理カメラ的なパラメータ(センサーサイズ/焦点距離/近遠クリップ)を保持する構造体
	struct Projection {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief 各パラメータを既定値で初期化する
		void Init();

		//! @brief 保持パラメータからプロジェクション行列を生成する
		//! @return プロジェクション行列
		Matrix4x4 ToProjection() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2f sensor; /*_mm*/
		float focal;     /*_mm*/
		float nearZ;
		float farZ;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CameraComponent(EntityBehaviour* behaviour);
	~CameraComponent() override = default;

	//! @brief インスペクタにこのcomponentの編集UIを表示する (BaseComponentのoverride)
	void ShowComponentInspector() override;

	//* buffer option *//

	//! @brief 現在フレームのカメラ定数バッファのGPU仮想アドレスを取得する
	//! @return 定数バッファのGPU仮想アドレス
	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	//! @brief 前フレームのカメラ定数バッファのGPU仮想アドレスを取得する (モーションベクトル等で使用)
	//! @return 前フレーム定数バッファのGPU仮想アドレス
	const D3D12_GPU_VIRTUAL_ADDRESS GetPrevGPUVirtualAddress() const;

	//! @brief 現在/前フレーム用のダブルバッファを入れ替える
	void SwapBuffer();

	//* camera option *//

	//! @brief Transformからビュー行列を更新し, GPUへ転送する
	void UpdateView();

	//! @brief Projection設定からプロジェクション行列を更新し, GPUへ転送する
	void UpdateProj();

	//! @brief カメラの用途タグを取得する
	//! @return タグ
	Tag GetTag() const { return tag_; }

	//! @brief カメラの用途タグを設定する
	//! @param[in] tag 設定するタグ
	void SetTag(Tag tag) { tag_ = tag; }

	//* getter *//

	//! @brief カメラ行列一式を取得する
	//! @return Camera構造体への参照
	const Camera& GetCamera() const;

	//! @brief プロジェクション設定を取得する
	//! @return Projectionへの参照
	const Projection& GetProjection() const { return projection_; }
	//! @brief プロジェクション設定を取得する (非const版)
	//! @return Projectionへの参照
	Projection& GetProjection() { return projection_; }

	//! @brief カメラのワールド座標位置を取得する
	//! @return ワールド位置
	Vector3f GetPosition() const;

	//* helper option *//

	//! @brief ワールド座標をNDC(正規化デバイス座標)へ変換する
	//! @param[in] point ワールド座標の点
	//! @return NDC座標
	Vector3f CalculateNDCPosition(const Vector3f& point) const;

	//! @brief NDC座標をワールド座標へ変換する
	//! @param[in] ndc NDC座標の点
	//! @return ワールド座標
	Vector3f CalculateWorldPosition(const Vector3f& ndc) const;

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

	Tag tag_               = Tag::None;
	Projection projection_ = {};

	std::array<std::unique_ptr<DxObject::ConstantBuffer<Camera>>, 2> buffers_;
	size_t currentIndex_ = 0;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateBuffer();

	void PushLineFrustum();

};

SXAVENGER_ENGINE_NAMESPACE_END
