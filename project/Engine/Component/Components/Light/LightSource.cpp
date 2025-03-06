#include "LightSource.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// InlineShadow structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void LightSource::InlineShadow::Init() {
	strength = 1.0f;
	flag     = D3D12_RAY_FLAG_CULL_BACK_FACING_TRIANGLES | D3D12_RAY_FLAG_CULL_NON_OPAQUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
// LightSource class methods
////////////////////////////////////////////////////////////////////////////////////////////

void LightSource::CreateShadowBuffer() {
	shadow_ = std::make_unique<DimensionBuffer<InlineShadow>>();
	shadow_->Create(SxavengerSystem::GetDxDevice(), 1);
	shadow_->At(0).Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& LightSource::GetShadowBufferAddress() const {
	Assert(shadow_ != nullptr, "shadow buffer is not create.");
	return shadow_->GetGPUVirtualAddress();
}
