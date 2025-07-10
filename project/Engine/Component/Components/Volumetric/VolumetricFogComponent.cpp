#include "VolumetricFogComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// VolumetricFogComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void VolumetricFogComponent::Init() {
	parameter_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& VolumetricFogComponent::GetGPUVirtualAddress() const {
	Exception::Assert(parameter_ != nullptr, "volumetric fog parameter buffer is not created.");
	return parameter_->GetGPUVirtualAddress();
}
