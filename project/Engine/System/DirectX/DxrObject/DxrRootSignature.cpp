#include "DxrRootSignature.h"
SXAVENGER_ENGINE_USING
DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignature class methods
////////////////////////////////////////////////////////////////////////////////////////////

void LocalRootSignature::CreateRootSignature(DxObject::Device* device, const LocalRootSignatureDesc& desc) {
	rootSignature_ = desc.CreateLocalRootSignature(device->GetDevice());
	stride_        = desc.params.size() * kBufferStride;
	//!< virtual address, Handleともに8byteなので、sizeは8byteで固定

	CreateTable(desc);
}

void LocalRootSignature::CreateTable(const LocalRootSignatureDesc& desc) {
	table_.Resize(static_cast<uint32_t>(desc.params.size()));

	for (uint32_t i = 0; i < desc.params.size(); ++i) {
		const auto& param = desc.params[i];

		switch (param.ParameterType) {
			case D3D12_ROOT_PARAMETER_TYPE_CBV:
			case D3D12_ROOT_PARAMETER_TYPE_SRV:
			case D3D12_ROOT_PARAMETER_TYPE_UAV:
				table_.RegisterAddress(i);
				break;

			case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
				table_.RegisterHandle(i);
				break;

			default:
				StreamLogger::Exception("invalid parameter type."); //!< 32bit
				break;
		}
	}
}
