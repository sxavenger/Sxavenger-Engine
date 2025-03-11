#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxUnorderedDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/Content/Animation/SkinCluster.h>
#include <Engine/Asset/Model/Model.h>

//* lib
#include <Lib/VertexStructure.h>

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

		void Create(const DirectXThreadContext* context, const Model::AssimpMesh& mesh);

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* input mesh *//

		std::unique_ptr<DxObject::VertexUnorderedDimensionBuffer<MeshVertexData>> vertex;
		//std::unique_ptr<> index;

		DxrObject::BottomLevelAS bottomLevelAS;

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		void CreateVetex(const Model::AssimpMesh& mesh);

		void CreateBottomLevelAS(const DirectXThreadContext* context, const Model::AssimpMesh& mesh);

	};


public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkinnedMeshRendererComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~SkinnedMeshRendererComponent() override = default;

	void Create(const std::shared_ptr<Model>& model, uint32_t index);

	void UpdateAnimation(const Animation& animation, TimePointf<TimeUnit::second> time, bool isLoop = true);

	void UpdateTransitionAnimation(
		const Animation& before, TimePointf<TimeUnit::second> beforeTime, bool isLoopBefore,
		const Animation& after, TimePointf<TimeUnit::second> afterTime, bool isLoopAfter,
		float t
	);

	//* getter *//

	const TransformComponent* GetTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Skeleton skeleton_;

	InputSkinnedMesh mesh_;
	SkinCluster cluster_;

	//!< hack: animationをここでするのはどうか
	//!< 理想はanimator componentなどを作ってそこで管理する

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateCluster(const Model::AssimpMesh& mesh);

};
