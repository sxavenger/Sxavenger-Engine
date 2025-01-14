#include "DxrShaderTable.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// RecorderSize structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderTable::RecorderSize::AlignmentRecoder(UINT align) {
	raygeneration = Alignment(raygeneration, align);
	miss          = Alignment(miss, align);
	hitgroup      = Alignment(hitgroup, align);
}

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderTable class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderTable::Init(DxObject::Device* device) {
	resource_ = DxObject::CreateBufferResource(
		device->GetDevice(),
		D3D12_HEAP_TYPE_UPLOAD,
		kShaderTableSize,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);
	//!< 毎フレームbuffer作成できないので、resourceは固定で作成しておく
}

void ShaderTable::Register() {

	// shader単体のsizeの設定
	RecorderSize recorderSize = {};
	//* raygeneration
	recorderSize.raygeneration = kShaderRecordSize; //!< raygenerationは1つ


	// todo:
}
