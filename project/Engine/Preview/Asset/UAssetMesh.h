#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "UBaseAsset.h"

//* engine
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/Content/InputGeometry/InputMesh.h>
#include <Engine/Content/Animation/JointWeight.h>

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
// UAssetMesh class
////////////////////////////////////////////////////////////////////////////////////////////
class UAssetMesh final
	: public UBaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UAssetMesh(const Uuid& id) : UBaseAsset(id) {}
	~UAssetMesh() override = default;

	void Setup(const aiMesh* mesh);

	void Update(const DirectXQueueContext* context);

	//* inspector option *//

	void ShowInspector() override;

	//* asset option *//

	void BindIABuffer(const DirectXQueueContext* context) const;

	void DrawCall(const DirectXQueueContext* context, UINT instanceCount = 1) const;

	//* getter *//

	const InputMesh& GetInputMesh() const;
	InputMesh& GetInputMesh();

	const InputMesh::InputVertex* GetInputVertex() const { return GetInputMesh().GetVertex(); }
	const InputMesh::InputIndex* GetInputIndex() const { return GetInputMesh().GetIndex(); }

	const std::unordered_map<std::string, JointWeightData>& GetJointWeights() const { return jointWeights_; }

	const std::string& GetName() const { return name_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* mesh data
	InputMesh                                        input_;
	std::optional<uint32_t>                          materialIndex_;
	std::unordered_map<std::string, JointWeightData> jointWeights_;
	std::string                                      name_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper convert methods *//

	static Vector3f ConvertNormal(const aiVector3D& aiVector);
	static Vector3f ConvertPosition3(const aiVector3D& aiVector);
	static Vector4f ConvertPosition4(const aiVector3D& aiVector);
	static Quaternion ConvertQuaternion(const aiQuaternion& aiQuaternion);

};
