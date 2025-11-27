#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"
#include "../LightCommon.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkyAtmosphereComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class SkyAtmosphereComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkyAtmosphereComponent(MonoBehaviour* behaviour);
	~SkyAtmosphereComponent() override = default;

	void CreateTransmittance(const DirectXQueueContext* context);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* precomputed textures *//

	std::pair<ComPtr<ID3D12Resource>, DxObject::Descriptor> transmittance_;
	std::pair<ComPtr<ID3D12Resource>, DxObject::Descriptor> multipleScattering_;

	//* pipeline state *//

	DxObject::ReflectionComputePipelineState pipeline1_;
	DxObject::ReflectionComputePipelineState pipeline2_;

};
