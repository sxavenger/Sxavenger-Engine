#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"
#include "Model.h"

//* engine
#include <Engine/Component/Entity/MonoBehaviour.h> //!< fixme: assetがcomponentを持つのはおかしい

////////////////////////////////////////////////////////////////////////////////////////////
// AssetModel class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetModel
	: public BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetModel()  = default;
	~AssetModel() = default;

	//* async task option *//

	void Load(_MAYBE_UNUSED const DirectXQueueContext* context) override;

	AsyncExecution GetAsyncExecution() const override { return AsyncExecution::None; }

	//* inspector option *//

	void ShowInspector() override;

	//* material option *//

	void UpdateMaterials();

	//* MonoBehavior option *//

	void CreateStaticMeshBehaviour(MonoBehaviour* root);

	std::unique_ptr<MonoBehaviour> CreateStaticMeshBehaviour(const std::string& name = "static mesh");

	void CreateStaticNodeMeshBehaviour(MonoBehaviour* root);

	std::unique_ptr<MonoBehaviour> CreateStaticNodeMeshBehaviour(const std::string& name = "static node mesh");

	void CreateSkinnedMeshBehaviour(MonoBehaviour* root);

	std::unique_ptr<MonoBehaviour> CreateSkinnedMeshBehaviour(const std::string& name = "skinned mesh");

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Model model_;

	// material
	std::vector<std::unique_ptr<Material>> materials_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::unique_ptr<Material> CreateMaterial(const Model::AssimpMaterial& material);

	void CreateMaterials();

	void UpdateMaterial(const Model::AssimpMaterial& material, Material* result) const;

};
