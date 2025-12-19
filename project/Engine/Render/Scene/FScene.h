#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DirectX/DxrObject/DxrStateObjectContext.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/DirectX/DirectXAlignment.h>
#include <Engine/Components/Component/ComponentStorage.h>
#include <Engine/Components/Component/MeshRenderer/MeshRendererComponent.h>
#include <Engine/Components/Component/MeshRenderer/SkinnedMeshRendererComponent.h>
#include <Engine/Components/Component/Light/Punctual/DirectionalLightComponent.h>
#include <Engine/Components/Component/Light/Punctual/PointLightComponent.h>
#include <Engine/Components/Component/Light/Punctual/SpotLightComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

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

	void SetupTopLevelAS(const DirectXQueueContext* context);

	const DxrObject::TopLevelAS& GetTopLevelAS() const { return topLevelAS_; }

	//* state object option *//

	void SetupStateObject();

	//* light container option * //

	void SetupLightContainer();

	void SetupDirectionalLight();
	void SetupPointLight();
	void SetupSpotLight();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* raytracing parameter *//

	DxrObject::TopLevelAS topLevelAS_;

	//* light container *//

	std::unique_ptr<DxObject::ConstantBuffer<uint32_t>>                              directionalLightCount_;
	std::unique_ptr<DxObject::DimensionBuffer<TransformationMatrix>>                 directionalLightTransforms_;
	std::unique_ptr<DxObject::DimensionBuffer<DirectionalLightComponent::Parameter>> directionalLightParams_;

	std::unique_ptr<DxObject::ConstantBuffer<uint32_t>>                        pointLightCount_;
	std::unique_ptr<DxObject::DimensionBuffer<TransformationMatrix>>           pointLightTransforms_;
	std::unique_ptr<DxObject::DimensionBuffer<PointLightComponent::Parameter>> pointLightParams_;

	std::unique_ptr<DxObject::ConstantBuffer<uint32_t>>                       spotLightCount_;
	std::unique_ptr<DxObject::DimensionBuffer<TransformationMatrix>>          spotLightTransforms_;
	std::unique_ptr<DxObject::DimensionBuffer<SpotLightComponent::Parameter>> spotLightParams_;

	//=========================================================================================
	// friend class
	//=========================================================================================
	//!< HACK
	friend class FRenderPassDeferredLighting;

};

SXAVENGER_ENGINE_NAMESPACE_END
