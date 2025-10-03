#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* mesh renderer
#include "MeshRendererCommon.h"

//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"
#include "../Armature/ArmatureComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxUnorderedDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/Content/Animation/SkinCluster.h>
#include <Engine/Preview/Asset/UAssetMesh.h>
#include <Engine/Preview/Asset/UAssetMaterial.h>
#include <Engine/Preview/Asset/UAssetParameter.h>

//* lib
#include <Lib/Sxl/Flag.h>
#include <Lib/Adapter/Uuid/Uuid.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkinnedMeshRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class SkinnedMeshRendererComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// InputSkinnedMesh structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct InputSkinnedMesh {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// using
		////////////////////////////////////////////////////////////////////////////////////////////

		using InputUnorderedVertex = DxObject::VertexUnorderedDimensionBuffer<MeshVertexData>;

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(const DirectXQueueContext* context, const std::shared_ptr<UAssetMesh>& mesh);

		void UpdateBottomLevelAS(const DirectXQueueContext* context);

		const DxrObject::BottomLevelAS* GetBottomLevelAS() const { return &bottomLevelAS; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* input mesh vertex *//

		std::unique_ptr<InputUnorderedVertex> vertex;
		//std::unique_ptr<> index;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* input mesh blas *//

		DxrObject::BottomLevelAS bottomLevelAS;

		//* parameter *//

		bool isCreateMesh = false;

		//=========================================================================================
		// private methods
		//=========================================================================================

		void CreateVetex(const std::shared_ptr<UAssetMesh>& mesh);

		void CreateBottomLevelAS(const DirectXQueueContext* context, const std::shared_ptr<UAssetMesh>& mesh);

	};


public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkinnedMeshRendererComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~SkinnedMeshRendererComponent() override = default;

	void ShowComponentInspector() override;

	void CreateMesh(const Uuid& referenceMesh);
	void CreateMesh(const std::shared_ptr<UAssetMesh>& referenceMesh);

	void SetMaterial(const Uuid& id) { material_ = id; }
	void SetMaterial(const std::shared_ptr<UAssetMaterial>& material) { material_ = material; }

	void Skinning();

	void Update(const DirectXQueueContext* context);

	//* option *//

	void BindIABuffer(const DirectXQueueContext* context) const;

	void DrawCall(const DirectXQueueContext* context, uint32_t instanceCount = 1) const;

	void SetEnable(bool isEnable = true) { isEnable_ = isEnable; }

	//* getter *//

	std::shared_ptr<UAssetMaterial> GetMaterial() const;
	const UAssetParameter<UAssetMaterial>& GetMaterialParameter() const { return material_; }

	const InputSkinnedMesh::InputUnorderedVertex* GetInputVertex() const { return mesh_.vertex.get(); }
	const InputMesh::InputIndex* GetInputIndex() const { return referenceMesh_.Require()->GetInputIndex(); }

	const DxrObject::BottomLevelAS* GetBottomLevelAS() const { return mesh_.GetBottomLevelAS(); }

	const TransformComponent* GetTransform() const;

	uint8_t GetMask() const { return mask_.Get(); }

	bool IsEnable() const { return !material_.Empty() && isEnable_;}

	//* json option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* reference *//

	UAssetParameter<UAssetMesh> referenceMesh_;
	UAssetParameter<UAssetMaterial> material_;

	InputSkinnedMesh mesh_;
	SkinCluster cluster_;

	Sxl::Flag<MeshInstanceMask> mask_ = MeshInstanceMask::Default;

	bool isEnable_ = true;

	//!< todo: instance描画

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateCluster();

	const ArmatureComponent* GetArmatureComponent() const;

};
