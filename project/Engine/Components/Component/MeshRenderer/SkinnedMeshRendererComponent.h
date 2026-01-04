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
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxUnorderedDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/Graphics/Animation/SkinCluster.h>
#include <Engine/Assets/Asset/AssetMesh.h>
#include <Engine/Assets/Asset/AssetMaterial.h>
#include <Engine/Assets/Asset/AssetParameter.h>

//* lib
#include <Lib/Sxl/Flag.h>
#include <Lib/Adapter/Uuid/Uuid.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SkinnedMeshRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Animation付きメッシュ描画component.
class SkinnedMeshRendererComponent final
	: public BaseComponent, public MeshRendererCommon {
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

		void Create(const DirectXQueueContext* context, const std::shared_ptr<AssetMesh>& mesh);

		void UpdateBottomLevelAS(const DirectXQueueContext* context);

		const DxrObject::BottomLevelAS* GetBottomLevelAS() const { return &bottomLevelAS; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* input mesh vertex *//

		std::unique_ptr<InputUnorderedVertex> vertex;

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

		void CreateVetex(const std::shared_ptr<AssetMesh>& mesh);

		void CreateBottomLevelAS(const DirectXQueueContext* context, const std::shared_ptr<AssetMesh>& mesh);

	};


public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkinnedMeshRendererComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) {}
	~SkinnedMeshRendererComponent() override = default;

	void ShowComponentInspector() override;

	void CreateMesh(const Uuid& referenceMesh);
	void CreateMesh(const std::shared_ptr<AssetMesh>& referenceMesh);

	void SetMaterial(const Uuid& id) { material_ = id; }
	void SetMaterial(const std::shared_ptr<AssetMaterial>& material) { material_ = material; }

	//! @brief スキニング処理を行う
	void Skinning();

	void Update(const DirectXQueueContext* context);

	//* option *//

	void BindIABuffer(const DirectXQueueContext* context) const;

	void DrawCall(const DirectXQueueContext* context, uint32_t instanceCount = 1) const;

	//* getter *//

	std::shared_ptr<AssetMaterial> GetMaterial() const;
	const AssetParameter<AssetMaterial>& GetMaterialParameter() const { return material_; }

	const InputSkinnedMesh::InputUnorderedVertex* GetInputVertex() const { return mesh_.vertex.get(); }
	const InputMesh::InputIndex* GetInputIndex() const { return referenceMesh_.Require()->GetInputIndex(); }

	const DxrObject::BottomLevelAS* GetBottomLevelAS() const { return mesh_.GetBottomLevelAS(); }

	uint8_t GetMask() const { return mask_.Get(); }

	bool IsEnable() const { return !material_.Empty() && isEnable_;}

	//* behaviour option *//

	//! @brief TransformComponentを取得
	//! @return TransformComponentのptr
	//! @throw TransformComponentがEntityに存在しない場合に例外をスローする
	const TransformComponent* RequireTransform() const;

	//* json option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* reference *//

	AssetParameter<AssetMesh> referenceMesh_;
	AssetParameter<AssetMaterial> material_;

	InputSkinnedMesh mesh_;
	SkinCluster cluster_;

	//!< todo: instance描画

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateCluster();

	const ArmatureComponent* GetArmatureComponent() const;

};

SXAVENGER_ENGINE_NAMESPACE_END
