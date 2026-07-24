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

	void Setup(const DirectXQueueContext* context, const aiMesh* mesh);

	//* mesh option *//

	void BindInputAssembler(const DirectXQueueContext* context) const;

	void DrawCall(const DirectXQueueContext* context, UINT instanceCount = 1) const;

	const InputMesh& GetInputMesh() const;

	const InputMesh::InputVertex* GetInputVertex() const { return GetInputMesh().GetVertex(); }
	const InputMesh::InputIndex* GetInputIndex() const { return GetInputMesh().GetIndex(); }

	const std::unordered_map<std::string, JointWeightData>& GetJointWeights() const { return jointWeights_; }
	// hack: meshの読み込みが完了してから取得する.

	//* convert methods *//

	static Vector3f ConvertVector3(const aiVector3D& aiVector);

	static Vector4f ConvertVector4(const aiVector3D& aiVector);

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
