#include "DxUnorderedBufferResource.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BaseUnorderedBufferResource base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseUnorderedBufferResource::Init(Devices* devices, uint32_t indexSize, size_t structureSize) {
	// 引数の確認
	Assert(structureSize != 0 || indexSize != 0);

	// sizeの保存
	indexSize_     = indexSize;
	structureSize_ = static_cast<UINT>(structureSize);

	// deviceの取り出し
	auto device = devices->GetDevice();

	// resourceの生成
	uavResource_ = CreateBufferResource(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		indexSize_ * structureSize_,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);

	/*writeResource_ = CreateBufferResource(
		device,
		D3D12_HEAP_TYPE_UPLOAD,
		indexSize_ * structureSize_,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);

	readBackResource_ = CreateBufferResource(
		device,
		D3D12_HEAP_TYPE_READBACK,
		indexSize_ * structureSize_,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST
	);*/
	
	//!< mapは各派生クラスで行う
}

void BaseUnorderedBufferResource::Term() {
}
