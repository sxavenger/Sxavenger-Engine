#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* mesh renderer
#include "MeshRendererCommon.h"

//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Assets/Asset/AssetMesh.h>
#include <Engine/Assets/Asset/AssetMaterial.h>
#include <Engine/Assets/Asset/AssetParameter.h>

//* lib
#include <Lib/Sxl/Flag.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// MeshRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 3Dメッシュの描画component.
class MeshRendererComponent final
	: public BaseComponent, public MeshRendererCommon {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MeshRendererComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) {}
	~MeshRendererComponent() override = default;

	//! @brief インスペクタにこのcomponentの編集UIを表示する (BaseComponentのoverride)
	void ShowComponentInspector() override;

	//* setter *//

	//! @brief 描画するメッシュをuuid参照で設定する
	//! @param[in] mesh メッシュAssetのuuid
	void SetMesh(const Uuid& mesh) { mesh_ = mesh; }
	//! @brief 描画するメッシュを直接指定して設定する
	//! @param[in] mesh メッシュAsset
	void SetMesh(const std::shared_ptr<AssetMesh>& mesh) { mesh_ = mesh; }

	//! @brief 使用するマテリアルをuuid参照で設定する
	//! @param[in] material マテリアルAssetのuuid
	void SetMaterial(const Uuid& material) { material_ = material; }
	//! @brief 使用するマテリアルを直接指定して設定する
	//! @param[in] material マテリアルAsset
	void SetMaterial(const std::shared_ptr<AssetMaterial>& material) { material_ = material; }

	//* getter *//

	//! @brief 描画可能な状態(メッシュ/マテリアルが揃っている)かを返す
	//! @retval true  描画可能
	//! @retval false 描画不可
	bool IsEnable() const;

	//! @brief 設定中のメッシュAssetを取得する
	//! @return メッシュAsset
	std::shared_ptr<AssetMesh> GetMesh() const;
	//! @brief 設定中のマテリアルAssetを取得する
	//! @return マテリアルAsset
	std::shared_ptr<AssetMaterial> GetMaterial() const;

	//! @brief メッシュのAssetParameter(参照情報)を取得する
	//! @return メッシュのAssetParameterへの参照
	const AssetParameter<AssetMesh>& GetMeshParameter() const { return mesh_; }
	//! @brief マテリアルのAssetParameter(参照情報)を取得する
	//! @return マテリアルのAssetParameterへの参照
	const AssetParameter<AssetMaterial>& GetMaterialParameter() const { return material_; }

	//* behaviour option *//

	//! @brief TransformComponentを取得
	//! @throw TransformComponentがEntityに存在しない場合に例外をスローする
	const TransformComponent* RequireTransform() const;

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

	//* parameter *//

	AssetParameter<AssetMesh> mesh_;
	AssetParameter<AssetMaterial> material_;

};

SXAVENGER_ENGINE_NAMESPACE_END
