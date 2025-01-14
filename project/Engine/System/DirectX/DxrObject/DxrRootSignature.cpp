#include "DxrRootSignature.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignature class methods
////////////////////////////////////////////////////////////////////////////////////////////

void LocalRootSignature::CreateRootSignature(DxObject::Device* device, const LocalRootSignatureDesc& desc) {
	rootSignature_ = desc.CreateLocalRootSignature(device->GetDevice());
	size_ = desc.params.size() * kBufferStride;
	//!< virtual address, Handleともに8byteなので、sizeは8byteで固定
}
