#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "BaseContent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Graphics/Animation/BornNode.h>

//* lib
#include <Lib/Math/Vector3.h>
#include <Lib/Math/Vector4.h>
#include <Lib/Math/Quaternion.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ContentModel class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief assimpでモデルファイルを読み込み, メッシュ/マテリアル/スケルトンをAsset化するContent
class ContentModel final
	: public BaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ContentModel() : BaseContent(Async::Execution::Cpu) {}

	~ContentModel() override = default;

	//* content option *//

	//! @brief 読み込み対象ファイルとパラメータを関連付ける (BaseContentのoverride)
	void Attach(const std::filesystem::path& filepath, const std::any& parameter) override;

	//! @brief モデルを読み込み, メッシュ/マテリアル/スケルトンをAssetStorageへ登録する (BaseContentのoverride)
	void Load(MAYBE_UNUSED const DirectXQueueContext* context) override;

	//* parameter option *//

	//! @brief 読み込んだメッシュ数を取得する
	//! @return メッシュ数
	size_t GetMeshCount() const { return meshes_.size(); }

	//! @brief ボーン階層のルートノードを取得する
	//! @return ルートノードへの参照
	const BornNode& GetRoot() const { return root_; }

	//* id option *//

	//! @brief 指定indexのメッシュAssetのuuidを取得する
	//! @param[in] index メッシュのindex
	//! @return メッシュAssetのuuid
	const Uuid& GetMeshId(size_t index) const { return meshes_[index]; }

	//! @brief 指定indexのマテリアルAssetのuuidを取得する
	//! @param[in] index マテリアルのindex
	//! @return マテリアルAssetのuuid
	const Uuid& GetMaterialId(size_t index) const { return materials_[index]; }

	//! @brief 指定メッシュに対応するマテリアルAssetのuuidを取得する
	//! @param[in] meshIndex メッシュのindex
	//! @return 対応するマテリアルAssetのuuid
	const Uuid& GetMeshToMaterialId(size_t meshIndex) const { return materials_[materialIndices_[meshIndex]]; }

	//! @brief スケルトンAssetのuuidを取得する
	//! @return スケルトンAssetのuuid
	const Uuid& GetSkeletonId() const { return skeleton_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<Uuid> meshes_;
	std::vector<Uuid> materials_;

	std::vector<uint32_t> materialIndices_; //!< mesh index to material index

	BornNode root_;
	Uuid skeleton_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void AttachUuid(const std::filesystem::path& filepath);

	//* load helper methods *//

	uint32_t GetOption();

	static std::shared_ptr<Assimp::Importer> LoadImporter(const std::filesystem::path& filepath, uint32_t option);

	void SetupMaterialIndices(const aiScene* aiScene);

	static BornNode ReadNode(const aiNode* node);

	void SetupBornNode(const aiNode* aiNode);

};

SXAVENGER_ENGINE_NAMESPACE_END
