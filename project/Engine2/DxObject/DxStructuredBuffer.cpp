#include <DxStructuredBuffer.h>

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// StructuredBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::StructuredBuffer::Init(BaseBufferResource* bufferResource) {
	// descriptorの取得
	descriptor_ = Sxavenger::GetCurrentDescriptor(DescriptorType::CBV_SRV_UAV);

	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
	desc.Format                     = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.FirstElement        = 0;
	desc.Buffer.NumElements         = bufferResource->GetIndexSize();
	desc.Buffer.StructureByteStride = bufferResource->GetStructureSize();
	desc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	Sxavenger::GetDevice()->CreateShaderResourceView(
		bufferResource->GetResource(),
		&desc,
		descriptor_.GetCPUHandle()
	);
}

void DxObject::StructuredBuffer::Term() {
	Sxavenger::DeleteDescriptor(descriptor_);
}
