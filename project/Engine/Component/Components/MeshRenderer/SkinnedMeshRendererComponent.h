#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"
#include "../Armature/ArmatureComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxUnorderedDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/Content/Animation/SkinCluster.h>
#include <Engine/Content/InputGeometry/InputMesh.h>
#include <Engine/Asset/Assets/Model/Model.h>

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

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(const DirectXThreadContext* context, const Model::AssimpMesh* mesh);

		void UpdateBottomLevelAS(const DirectXThreadContext* context);

		const DxrObject::BottomLevelAS* GetBottomLevelAS() const { return &bottomLevelAS; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* input mesh vertex *//

		std::unique_ptr<DxObject::VertexUnorderedDimensionBuffer<MeshVertexData>> vertex;
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

		void CreateVetex(const Model::AssimpMesh* mesh);

		void CreateBottomLevelAS(const DirectXThreadContext* context, const Model::AssimpMesh* mesh);

	};


public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkinnedMeshRendererComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~SkinnedMeshRendererComponent() override = default;

	void CreateMesh(const Model::AssimpMesh* mesh);

	void SetMaterial(const Material* material) { material_ = material; }

	void Skinning();

	//* option *//

	void BindIABuffer(const DirectXThreadContext* context) const;

	void DrawCall(const DirectXThreadContext* context, uint32_t instanceCount = 1) const;

	//* getter *//

	const Material* GetMaterial() const { return material_; }

	const InputSkinnedMesh& GetMesh() const { return mesh_; }
	InputSkinnedMesh& GetMesh() { return mesh_; }

	const TransformComponent* GetTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* reference *//

	const Model::AssimpMesh* referenceMesh_;

	const Material* material_; //!< materialのkeyを持たせてそれを設定させる...?

	InputSkinnedMesh mesh_;
	SkinCluster cluster_;

	//!< todo: instance描画

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateCluster();

	const ArmatureComponent* GetArmatureComponent() const;

};
