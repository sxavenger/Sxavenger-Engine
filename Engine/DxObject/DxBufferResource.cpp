#include "DxBufferResource.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BufferResourceBase class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::BaseBufferResource::Init(uint32_t indexSize, size_t structureSize) {
	assert((structureSize > 0)); //!< 引数の確認

	indexSize_     = indexSize;
	structureSize_ = static_cast<UINT>(structureSize);
}

void DxObject::BaseBufferResource::Term() {
	indexSize_     = NULL;
	structureSize_ = NULL;
}

bool DxObject::BaseBufferResource::CheckElementCount(uint32_t elementCount) {
	if (elementCount >= indexSize_) {
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// IndexBufferResource methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::IndexBufferResource::Init(DxObject::Devices* devices) {

	// deviceを取り出す
	ID3D12Device* device = devices->GetDevice();

	// 配列分のBufferResourceを生成
	resource_ = DxObjectMethod::CreateBufferResource(
		device,
		structureSize_ * indexSize_
	);

	// resourceをマッピング
	uint32_t* mappingTarget = nullptr;

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappingTarget));

	mappedDatas_ = { mappingTarget, indexSize_ };
}

void DxObject::IndexBufferResource::Term() {
}

////////////////////////////////////////////////////////////////////////////////////////////
// OffsetBufferResource class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::OffsetBufferResource::Init(Devices* devices) {

	// deviceを取り出す
	ID3D12Device* device = devices->GetDevice();

	// 配列分のBufferResourceを生成
	resource_ = DxObjectMethod::CreateBufferResource(
		device,
		structureSize_
	);

	resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedDatas_));
}

void DxObject::OffsetBufferResource::Term() {
}
