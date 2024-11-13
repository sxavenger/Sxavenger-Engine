#include "Material.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PBRMaterialBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PBRMaterialBuffer::Create() {
	buffer_ = std::make_unique<BufferResource<PBRMaterial>>(Sxavenger::GetDevicesObj(), 1);
	(*buffer_)[0].roughness = 0.0f;
	(*buffer_)[0].metallic  = 0.0f;
}

void PBRMaterialBuffer::Term() {
}

void PBRMaterialBuffer::Transfer() {
	if (buffer_ != nullptr) {
		(*buffer_)[0] = material;
	}
}

void PBRMaterialBuffer::SetImGuiCommand() {
	material.SetImGuiCommand();
	Transfer();
}

const D3D12_GPU_VIRTUAL_ADDRESS PBRMaterialBuffer::GetGPUVirtualAddress() const {
	Assert(buffer_ != nullptr, "PBRMaterial buffer is nullptr.");
	return buffer_->GetGPUVirtualAddress();
}
