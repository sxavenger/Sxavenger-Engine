#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include <Engine/Component/Components/ComponentStorage.h>
#include <Engine/Component/Components/MeshRenderer/MeshRendererComponent.h>
#include <Engine/Component/Components/MeshRenderer/SkinnedMeshRendererComponent.h>
#include <Engine/Component/Components/Light/Punctual/DirectionalLightComponent.h>

//* engine
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DirectX/DxrObject/DxrStateObjectContext.h>
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/DirectX/DirectXAlignment.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FScene class
////////////////////////////////////////////////////////////////////////////////////////////
class FScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	//* tlas option *//

	void SetupTopLevelAS(const DirectXThreadContext* context);

	const DxrObject::TopLevelAS& GetTopLevelAS() const { return topLevelAS_; }

	//* state object option *//

	void SetupStateObject();

	const DxrObject::StateObjectContext& GetStateObjectContext() const { return stateObjectContext_; }

	//* light container option * //

	void SetupLightContainer();

	void SetupDirectionalLight();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* raytracing parameter *//

	DxrObject::TopLevelAS topLevelAS_;
	DxrObject::StateObjectContext stateObjectContext_;

	//* light container *//

	std::unique_ptr<DxObject::DimensionBuffer<uint32_t>>                                directionalLightCount_;
	std::unique_ptr<DxObject::DimensionBuffer<TransformationMatrix>>                    directionalLightTransforms_;
	std::unique_ptr<DxObject::DimensionBuffer<DirectionalLightComponent::Parameter>>    directionalLightParams_;
	std::unique_ptr<DxObject::DimensionBuffer<DirectionalLightComponent::InlineShadow>> directionalLightShadowParams_;

	//=========================================================================================
	// friend class
	//=========================================================================================
	//!< HACK
	friend class FSceneRenderer;

};
