#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/Graphics/InputGeometry/InputMesh.h>
#include <Engine/Graphics/Animation/JointWeight.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <unordered_map>
#include <optional>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// AssetMesh class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief assimpのaiMeshからメッシュ(頂点/インデックス/スキン情報)を読み込み, 描画可能な形で保持するAsset
class AssetMesh final
	: public BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	AssetMesh(const Uuid& id) : BaseAsset(id) {}

	~AssetMesh() override = default;

	//* setup option *//

	//! @brief aiMeshを解析し, 頂点/インデックス/スキン情報を構築する
	//! @param[in] context DirectXのキューコンテキスト (Compute Queue以上が必要)
	//! @param[in] mesh    読み込み元のassimpメッシュ
	//! @note 別スレッドから呼ばれる想定. 完了はBaseAsset::WaitComplete()で待機できる
	void Setup(const DirectXQueueContext* context, const aiMesh* mesh);

	//* mesh option *//

	//! @brief 頂点バッファ/インデックスバッファをInputAssemblerにバインドする
	//! @param[in] context DirectXのキューコンテキスト
	void BindInputAssembler(const DirectXQueueContext* context) const;

	//! @brief メッシュのドローコールを発行する
	//! @param[in] context       DirectXのキューコンテキスト
	//! @param[in] instanceCount 描画するインスタンス数
	void DrawCall(const DirectXQueueContext* context, UINT instanceCount = 1) const;

	//! @brief 入力メッシュ(頂点/インデックス情報)を取得する
	//! @return 入力メッシュへの参照
	//! @note 読み込み完了まで内部で待機する
	const InputMesh& GetInputMesh() const;

	//! @brief 頂点配列の先頭ポインタを取得する
	//! @return 頂点配列へのポインタ
	const InputMesh::InputVertex* GetInputVertex() const { return GetInputMesh().GetVertex(); }

	//! @brief インデックス配列の先頭ポインタを取得する
	//! @return インデックス配列へのポインタ
	const InputMesh::InputIndex* GetInputIndex() const { return GetInputMesh().GetIndex(); }

	//! @brief スキニング用のジョイントウェイト情報を取得する
	//! @return ジョイント名をキーとしたウェイトデータのマップ
	const std::unordered_map<std::string, JointWeightData>& GetJointWeights() const { return jointWeights_; }
	// hack: meshの読み込みが完了してから取得する.

	//* convert methods *//

	//! @brief assimpのaiVector3Dを左手座標系のVector3fへ変換する
	//! @param[in] aiVector assimpのベクトル(右手座標系)
	//! @return 左手座標系に変換したVector3f
	static Vector3f ConvertVector3(const aiVector3D& aiVector);

	//! @brief assimpのaiVector3Dを左手座標系のVector4f(w=1.0)へ変換する
	//! @param[in] aiVector assimpのベクトル(右手座標系)
	//! @return 左手座標系に変換したVector4f
	static Vector4f ConvertVector4(const aiVector3D& aiVector);

	//! @brief assimpのaiQuaternionを左手座標系のQuaternionへ変換する
	//! @param[in] aiQuaternion assimpのクォータニオン(右手座標系)
	//! @return 左手座標系に変換したQuaternion
	static Quaternion ConvertQuaternion(const aiQuaternion& aiQuaternion);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* mesh data
	InputMesh                                        input_;
	std::unordered_map<std::string, JointWeightData> jointWeights_;

	//=========================================================================================
	// private methods
	//=========================================================================================

};

SXAVENGER_ENGINE_NAMESPACE_END
