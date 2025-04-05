#include "IBLParameter.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure
////////////////////////////////////////////////////////////////////////////////////////////

void IBLParameter::Parameter::Init() {
	type = Type::None;
	index = NULL;
	value = kWhite4<>;
}

////////////////////////////////////////////////////////////////////////////////////////////
// IBLParameter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void IBLParameter::Create() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

void IBLParameter::SetNone() {
	parameter_->At(0).type = IBLParameter::Type::None;
}

void IBLParameter::SetColor(const Color4f& color) {
	parameter_->At(0).type  = IBLParameter::Type::Value;
	parameter_->At(0).value = color;
}

void IBLParameter::SetTextureCube(uint32_t index) {
	parameter_->At(0).type  = IBLParameter::Type::TextureCube;
	parameter_->At(0).index = index;
}

void IBLParameter::SetTexture2D(uint32_t index) {
	parameter_->At(0).type  = IBLParameter::Type::Texture2D;
	parameter_->At(0).index = index;
}

const D3D12_GPU_VIRTUAL_ADDRESS& IBLParameter::GetGPUVirtualAddress() const {
	Assert(parameter_ != nullptr, "ibl parameter buffer is not create.");
	return parameter_->GetGPUVirtualAddress();
}
