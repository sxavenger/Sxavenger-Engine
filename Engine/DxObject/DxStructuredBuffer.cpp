#include <DxStructuredBuffer.h>

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

////////////////////////////////////////////////////////////////////////////////////////////
// StructuredBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::StructuredBuffer::Init(BaseBufferResource* bufferResource) {
	// descriptorの取得
	descriptor_ = MyEngine::GetCurrentDescripor(DescriptorType::SRV);

	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
	desc.Format                     = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.FirstElement        = 0;
	desc.Buffer.NumElements         = bufferResource->GetIndexSize();
	desc.Buffer.StructureByteStride = bufferResource->GetStructureSize();
	desc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	MyEngine::GetDevicesObj()->GetDevice()->CreateShaderResourceView(
		bufferResource->GetResource(),
		&desc,
		descriptor_.handleCPU
	);
}

void DxObject::StructuredBuffer::Term() {
	MyEngine::EraseDescriptor(descriptor_);
}
